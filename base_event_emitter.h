#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "base_callback.h"

namespace daw {
	namespace nodepp {
		namespace base {
			namespace impl {
				template<typename T>
				T const & make_const( T const & val ) {
					return val;
				}
			}

			// Enum must have removeListener and newListener
			class EventEmitter {
				using listener_list_t = std::vector < std::pair<bool, Callback> > ;
				std::unordered_map<std::string, listener_list_t> m_listeners;
				size_t m_max_listeners;
				static char const * const c_new_listener_event;
				static char const * const c_remove_listener_event;

				bool at_max_listeners( std::string event );
			protected:
				virtual bool event_is_valid( std::string const & event ) const;

			public:
				using callback_id_t = Callback::id_t;

				EventEmitter( );

				virtual ~EventEmitter( );
					
				template<typename Listener>
				callback_id_t add_listener( std::string event, Listener listener, bool run_once = false ) {
					if( !at_max_listeners( event ) ) {
						auto callback = Callback( listener );
						m_listeners[event].emplace_back( run_once, callback );
						emit( c_new_listener_event, event, callback );
						return callback.id( );
					} else {
						// TODO: implement logging to fail gracefully.  For now throw
						throw std::runtime_error( "Max listeners reached for event" );
					}
				}

				template<typename Listener>
				EventEmitter& on( std::string event, Listener& listener ) {
					add_listener( event, listener );
					return *this;
				}

				template<typename Listener>
				EventEmitter& once( std::string event, Listener& listener ) {
					add_listener( event, listener, true );
					return *this;
				}

				EventEmitter& remove_listener( std::string event, callback_id_t id );

				EventEmitter& remove_listener( std::string event, Callback listener );

				EventEmitter& remove_all_listeners( );

				EventEmitter& remove_all_listeners( std::string event );

				EventEmitter& set_max_listeners( size_t max_listeners );

				listener_list_t listeners( std::string event );

				template<typename... Args>
				EventEmitter& emit( std::string event, Args&&... args ) {
					for( auto& callback : m_listeners[event] ) {
						callback.second.exec( std::forward<Args>( args )... );
					}
					daw::algorithm::erase_remove_if( m_listeners[event], []( std::pair<bool, Callback> const & item ) {
						return item.first;
					} );
					return *this;
				}

				static size_t listener_count( EventEmitter & emitter, std::string event );

				template<typename Listener, typename Action>
				auto rollback_event_on_exception( std::string event, Listener listener, Action action_to_try, bool run_listener_once = false ) -> decltype(action_to_try( )) {
					auto cb_id = add_listener( event, listener, run_listener_once );
					try {
						return action_to_try( );
					} catch( ... ) {
						// Rollback listener
						remove_listener( event, cb_id );
						std::rethrow_exception( std::current_exception( ) );
					}
				}

			};	// class EventEmitter
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
