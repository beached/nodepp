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

			struct IEventEmitter { 
				using listener_list_t = std::vector < std::pair<bool, Callback> > ;
				using listeners_t = std::unordered_map < std::string, listener_list_t > ;
				using callback_id_t = Callback::id_t;

				virtual ~IEventEmitter( ) = default;

				virtual std::vector<std::string> const & valid_events( ) const = 0;
				virtual void when_listener_added( std::function<void( std::string, Callback )> listener ) = 0;
				virtual void when_listener_removed( std::function<void( std::string, Callback )> listener ) = 0;
				virtual void when_error( std::function<void( base::Error )> listener ) = 0;
				virtual void when_next_listener_added( std::function<void( std::string, Callback )> listener ) = 0;
				virtual void when_next_listener_removed( std::function<void( std::string, Callback )> listener ) = 0;
				virtual void when_next_error( std::function<void( base::Error )> listener ) = 0;
				virtual void remove_listener( std::string event, callback_id_t id ) = 0;
				virtual void remove_listener( std::string event, Callback listener ) = 0;
				virtual void remove_all_listeners( ) = 0;
				virtual void remove_all_listeners( std::string event ) = 0;
				virtual void set_max_listeners( size_t max_listeners ) = 0;
				virtual listeners_t & listeners( ) = 0;
				virtual listener_list_t listeners( std::string event ) = 0;
				virtual size_t listener_count( std::string event ) = 0;
				virtual void tap( std::function<void( std::string, size_t )> listner ) = 0;
				virtual void untap( ) = 0;
				virtual void emit_error( std::string description, std::string where ) = 0;
				virtual void emit_error( std::string where, base::Error child ) = 0;
				virtual void emit_error( boost::system::error_code const & err, std::string where ) = 0;
				virtual void emit_error( std::exception_ptr ex, std::string description, std::string where ) = 0;
				virtual void emit_new_listener( std::string event, Callback listener ) = 0;
				virtual void emit_remove_listener( std::string event, Callback listener ) = 0;
				virtual bool event_is_valid( std::string const & event ) const = 0;
				virtual bool at_max_listeners( std::string event ) = 0;

				template<typename Listener>
				callback_id_t add_listener( std::string event, Listener listener, bool run_once = false ) {
					if( event.empty( ) ) {
						throw std::runtime_error( "Empty event name passed to add_listener" );
					}
					if( !at_max_listeners( event ) ) {
						auto callback = Callback( listener );
						if( event != "newListener" ) {
							emit_new_listener( event, callback );
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
				void once( std::string event, Listener listener ) {
					add_listener( event, listener, true );
				}

				template<typename... Args>
				void emit( std::string event, Args&&... args ) {
					if( event.empty( ) ) {
						throw std::runtime_error( "Empty event name passed to emit" );
					}
					assert( daw::algorithm::contains( this->valid_events( ), event ) );
					auto& callbacks = listeners( )[event];
					if( !listeners( )["tap"].empty( ) ) {
						listeners( )["tap"][0].second.exec( event, callbacks.size( ) );
					}

					for( auto& callback : callbacks ) {
						if( !callback.second.empty( ) ) {
							callback.second.exec( std::forward<Args>( args )... );
						}
					}
					daw::algorithm::erase_remove_if( callbacks, []( std::pair<bool, Callback> const & item ) {
						return item.first;
					} );
				}
			
			};

			//////////////////////////////////////////////////////////////////////////
			/// Summary:	Allows for the dispatch of events to subscribed listeners
			///				Callbacks can be be c-style function pointers, lambda's or 
			///				std::function with the correct signature.
			///	Requires:	base::Callback
			///	
			class EventEmitter: public IEventEmitter {
				std::shared_ptr<std::unordered_map<std::string, listener_list_t>> m_listeners;
				size_t m_max_listeners;

				void emit_error( base::Error error );

			public:
				EventEmitter( );
				virtual ~EventEmitter( ) = default;
				EventEmitter( EventEmitter const & ) = default;
				EventEmitter( EventEmitter && other );
				EventEmitter& operator=(EventEmitter const &) = default;
				EventEmitter& operator=(EventEmitter && rhs);

				void swap( EventEmitter& rhs );

				virtual std::vector<std::string> const & valid_events( ) const override;

				virtual void when_listener_added( std::function<void( std::string, Callback )> listener ) override;
				virtual void when_listener_removed( std::function<void( std::string, Callback )> listener ) override;
				virtual void when_error( std::function<void( base::Error )> listener ) override;

				virtual void when_next_listener_added( std::function<void( std::string, Callback )> listener ) override;
				virtual void when_next_listener_removed( std::function<void( std::string, Callback )> listener ) override;
				virtual void when_next_error( std::function<void( base::Error )> listener ) override;

				virtual void remove_listener( std::string event, callback_id_t id ) override;

				virtual void remove_listener( std::string event, Callback listener ) override;

				virtual void remove_all_listeners( ) override;

				virtual void remove_all_listeners( std::string event ) override;

				virtual void set_max_listeners( size_t max_listeners ) override;
				virtual listeners_t & listeners( ) override;
				virtual listener_list_t listeners( std::string event ) override;
				virtual size_t listener_count( std::string event ) override;

				virtual void emit_error( std::string description, std::string where ) override;
				virtual void emit_error( std::string where, base::Error child ) override;
				virtual void emit_error( boost::system::error_code const & err, std::string where ) override;
				virtual void emit_error( std::exception_ptr ex, std::string description, std::string where ) override;

				virtual void emit_new_listener( std::string event, Callback listener ) override;
				virtual void emit_remove_listener( std::string event, Callback listener ) override;
				virtual bool at_max_listeners( std::string event ) override;
				virtual bool event_is_valid( std::string const & event ) const override;

				void run( );				
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
