#pragma once

#include <boost/asio/error.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "base_callback.h"
#include "base_error.h"
#include "base_service_handle.h"
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
			///	
			struct EventEmitter: std::enable_shared_from_this<EventEmitter> {
				using listener_list_t = std::vector < std::pair<bool, Callback> > ;
				using listeners_t = std::unordered_map < std::string, listener_list_t > ;
				using callback_id_t = Callback::id_t;
			private:
				std::shared_ptr<std::unordered_map<std::string, listener_list_t>> m_listeners;
				size_t m_max_listeners;

				void emit_error( base::Error error );

			public:
				virtual ~EventEmitter( ) = default;
				EventEmitter( EventEmitter const & ) = default;
				EventEmitter& operator=(EventEmitter const &) = default;
				EventEmitter( );
				EventEmitter( EventEmitter && other );
				EventEmitter& operator=(EventEmitter && rhs);
				void swap( EventEmitter& rhs );

				std::shared_ptr<EventEmitter> get_ptr( );

				virtual void on_listener_added( std::function<void( std::string, Callback )> listener );
				virtual void on_listener_removed( std::function<void( std::string, Callback )> listener );
				virtual void on_error( std::function<void( base::Error )> listener );

				virtual void on_next_listener_added( std::function<void( std::string, Callback )> listener );
				virtual void on_next_listener_removed( std::function<void( std::string, Callback )> listener );
				virtual void on_next_error( std::function<void( base::Error )> listener );

				virtual void remove_listener( std::string event, callback_id_t id );

				virtual void remove_listener( std::string event, Callback listener );

				virtual void remove_all_listeners( );

				virtual void remove_all_listeners( std::string event );

				virtual void set_max_listeners( size_t max_listeners );
				virtual listeners_t & listeners( );
				virtual listener_list_t listeners( std::string event );
				virtual size_t listener_count( std::string event );

				template<typename Listener>
				callback_id_t add_listener( std::string event, Listener listener, bool run_once = false ) {
					if( event.empty( ) ) {
						throw std::runtime_error( "Empty event name passed to add_listener" );
					}
					if( !at_max_listeners( event ) ) {
						auto callback = Callback( listener );
						if( event != "newListener" ) {
							emit_listener_added( event, callback );
						}
						listeners( )[event].emplace_back( run_once, callback );
						return callback.id( );
					} else {
						// TODO: implement logging to fail gracefully.  For now throw
						throw std::runtime_error( "Max listeners reached for event" );
					}
				}

				template<typename Listener>
				void on( std::string event, Listener listener ) {
					add_listener( event, listener );
				}

				template<typename Listener>
				void on_next( std::string event, Listener listener ) {
					add_listener( event, listener, true );
				}

				template<typename... Args>
				void emit( std::string event, Args&&... args ) {
					if( event.empty( ) ) {
						throw std::runtime_error( "Empty event name passed to emit" );
					}

					auto& callbacks = listeners( )[event];
					for( auto& callback : callbacks ) {
						if( !callback.second.empty( ) ) {
							callback.second.exec( std::forward<Args>( args )... );
						}
					}
					daw::algorithm::erase_remove_if( callbacks, []( std::pair<bool, Callback> const & item ) {
						return item.first;
					} );
				}

				virtual void emit_error( std::string description, std::string where );
				virtual void emit_error( std::string where, base::Error child );
				virtual void emit_error( boost::system::error_code const & err, std::string where );
				virtual void emit_error( std::exception_ptr ex, std::string description, std::string where );

				virtual void emit_listener_added( std::string event, Callback listener );
				virtual void emit_listener_removed( std::string event, Callback listener );
				virtual bool at_max_listeners( std::string event );
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
