#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <memory>
#include <utility>
#include <vector>
#include "base_callback.h"
#include "base_error.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace base {
			namespace impl {
				template<typename T>
				T const & make_const( T const & val ) {
					return val;
				}

				template<typename T>
				T append_vector( T v1, T const & v2 ) {
					v1.reserve( v1.size( ) + v2.size( ) );
					std::copy( std::begin( v2 ), std::end( v2 ), std::back_inserter( v1 ) );
					return v1;
				}
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary:	Allows for the dispatch of events to subscribed listeners
			///				Callbacks can be be c-style function pointers, lambda's or 
			///				std::function with the correct signature.
			///	Requires:	base::Callback
			class EventEmitter {
				using listener_list_t = std::vector < std::pair<bool, Callback> > ;
				using listeners_t = std::unordered_map < std::string, listener_list_t > ;

				std::shared_ptr<std::unordered_map<std::string, listener_list_t>> m_listeners;
				size_t m_max_listeners;

				bool at_max_listeners( std::string event );
				listeners_t & listeners( );
				listeners_t const & listeners( ) const;
				bool event_is_valid( std::string const & event ) const;		
			public:
				using callback_id_t = Callback::id_t;
			protected:
				template<typename Listener>
				callback_id_t add_listener( std::string event, Listener listener, bool run_once = false ) {
					if( !at_max_listeners( event ) ) {
						auto callback = Callback( listener );
						emit( "newListener", event, callback );
						listeners( )[event].emplace_back( run_once, callback );
						return callback.id( );
					} else {
						// TODO: implement logging to fail gracefully.  For now throw
						throw std::runtime_error( "Max listeners reached for event" );
					}
				}

// 				template<typename Listener>
// 				EventEmitter& on( std::string event, Listener listener ) {
// 					add_listener( event, listener );
// 					return *this;
// 				}
// 
// 				template<typename Listener>
// 				EventEmitter& once( std::string event, Listener listener ) {
// 					add_listener( event, listener, true );
// 					return *this;
// 				}

			public:
				virtual std::vector<std::string> const & valid_events( ) const;

				EventEmitter( );
				virtual ~EventEmitter( );
				EventEmitter( EventEmitter const & ) = default;
				EventEmitter& operator=(EventEmitter const &) = default;
				EventEmitter( EventEmitter && other );
				EventEmitter& operator=(EventEmitter && rhs);

				void swap( EventEmitter& rhs );


				virtual EventEmitter& on_newListener( std::function<void( std::string, Callback )> listener );
				virtual EventEmitter& on_removeListener( std::function<void( std::string, Callback )> listener );
				virtual EventEmitter& on_error( std::function<void( base::Error )> listener );

				virtual EventEmitter& once_newListener( std::function<void( std::string, Callback )> listener );
				virtual EventEmitter& once_removeListener( std::function<void( std::string, Callback )> listener );
				virtual EventEmitter& once_error( std::function<void( base::Error )> listener );

				EventEmitter& remove_listener( std::string event, callback_id_t id );

				EventEmitter& remove_listener( std::string event, Callback listener );

				EventEmitter& remove_all_listeners( );

				EventEmitter& remove_all_listeners( std::string event );

				EventEmitter& set_max_listeners( size_t max_listeners );

				listener_list_t listeners( std::string event );
				listener_list_t const listeners( std::string event ) const;
				size_t listener_count( std::string event );

				template<typename... Args>
				void emit( std::string event, Args&&... args ) {
					assert( daw::algorithm::contains( this->valid_events( ), event ) );
					for( auto& callback : listeners( )[event] ) {
						if( !callback.second.empty( ) ) {
							callback.second.exec( std::forward<Args>( args )... );
						}
					}
					daw::algorithm::erase_remove_if( listeners( )[event], []( std::pair<bool, Callback> const & item ) {
						return item.first;
					} );
				}				

			};	// class EventEmitter

			template<typename This, typename Listener, typename Action>
			static auto rollback_event_on_exception( This me, std::string event, Listener listener, Action action_to_try, bool run_listener_once = false ) -> decltype(action_to_try( )) {
				auto cb_id = me->add_listener( event, listener, run_listener_once );
				try {
					return action_to_try( );
				} catch( ... ) {
					// Rollback listener
					me->remove_listener( event, cb_id );
					std::rethrow_exception( std::current_exception( ) );
				}
			}
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
