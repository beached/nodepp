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
				struct EventEmitterImpl;
			}
			
			using EventEmitter = std::shared_ptr < impl::EventEmitterImpl > ;

			EventEmitter create_event_emitter( );

			namespace impl {
				//////////////////////////////////////////////////////////////////////////
				/// Summary:	Allows for the dispatch of events to subscribed listeners
				///				Callbacks can be be c-style function pointers, lambda's or 
				///				std::function with the correct signature.
				///	Requires:	base::Callback
				///	
				struct EventEmitterImpl: std::enable_shared_from_this < EventEmitterImpl > {
					using listener_list_t = std::vector < std::pair<bool, Callback> > ;
					using listeners_t = std::unordered_map < std::string, listener_list_t > ;
					using callback_id_t = Callback::id_t;
				private:
					std::shared_ptr<std::unordered_map<std::string, listener_list_t>> m_listeners;
					size_t m_max_listeners;
					EventEmitterImpl( );
				public:
					friend base::EventEmitter base::create_event_emitter( );

					~EventEmitterImpl( ) = default;
					EventEmitterImpl( EventEmitterImpl const & ) = default;
					EventEmitterImpl& operator=(EventEmitterImpl const &) = default;					
					EventEmitterImpl( EventEmitterImpl && other );
					EventEmitterImpl& operator=(EventEmitterImpl && rhs);
					void swap( EventEmitterImpl& rhs );

					std::shared_ptr<EventEmitterImpl> get_ptr( );

					void remove_listener( std::string event, callback_id_t id );

					void remove_listener( std::string event, Callback listener );

					void remove_all_listeners( );

					void remove_all_listeners( std::string event );

					void set_max_listeners( size_t max_listeners );
					listeners_t & listeners( );
					listener_list_t listeners( std::string event );
					size_t listener_count( std::string event );

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

					void emit_listener_added( std::string event, Callback listener );
					void emit_listener_removed( std::string event, Callback listener );

					bool at_max_listeners( std::string event );
				};	// class EventEmitterImpl
			}	// namespace impl
			
			//////////////////////////////////////////////////////////////////////////
			// Allows one to have the Events defined in event emitter
			template<typename Child>
			class StandardEvents {
				Child& child( ) {
					return *static_cast<Child*>(this);
				}

				EventEmitter& emitter( ) {
					return child( ).emitter( );
				}

				void emit_error( base::Error error ) {
					emitter( )->emit( "error", std::move( error ) );
				}
			public:
				//////////////////////////////////////////////////////////////////////////
				/// Summary: Callback is for when error's occur
				Child& on_error( std::function<void( base::Error )> listener ) {
					emitter( )->add_listener( "error", listener );
					return child( );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Callback is for the next error
				Child& on_next_error( std::function<void( base::Error )> listener ) {
					emitter( )->add_listener( "error", listener, true );
					return child( );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary:	Callback is called whenever a new listener is added for 
				///				any callback
				Child& on_listener_added( std::function<void( std::string, Callback )> listener ) {
					emitter( )->add_listener( "listener_added", listener );
					return child( );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary:	Callback is called when the next new listener is added
				///				for any callback
				Child& on_next_listener_added( std::function<void( std::string, Callback )> listener ) {
					emitter( )->add_listener( "listener_added", listener, true );
					return child( );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Callback is called whenever a listener is removed for 
				/// any callback
				Child& on_listener_removed( std::function<void( std::string, Callback )> listener ) {
					emitter( )->add_listener( "listener_removed", listener );
					return child( );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Callback is called the next time a listener is removed for 
				/// any callback
				Child& on_next_listener_removed( std::function<void( std::string, Callback )> listener ) {
					emitter( )->add_listener( "listener_removed", listener, true );
					return child( );
				}

			protected:
				//////////////////////////////////////////////////////////////////////////
				/// Summary: Emit an error event
				void emit_error( std::string description, std::string where ) {
					base::Error err( description );
					err.add( "where", std::move( where ) );

					emit_error( std::move( err ) );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Emit an error event
				void emit_error( std::string where, base::Error child ) {
					base::Error err( "Child Error" );
					err.add( "where", std::move( where ) );
					err.child( std::move( child ) );

					emit_error( std::move( err ) );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Emit an error event
				void emit_error( boost::system::error_code const & error, std::string where ) {
					base::Error err( error );
					err.add( "where", where );

					emit_error( std::move( err ) );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Emit an error event
				void emit_error( std::exception_ptr ex, std::string description, std::string where ) {
					base::Error err( description, ex );
					err.add( "where", where );

					emit_error( std::move( err ) );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary:	Emit an event with the callback and event name of a newly
				///				added event
				void emit_listener_added( std::string event, Callback listener ) {
					emitter( )->emit_listener_added( event, )
						emitter( )->emit( "listener_added", std::move( event ), std::move( listener ) );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary:	Emit an event with the callback and event name of an event
				///				that has been removed
				void emit_listener_removed( std::string event, Callback listener ) {
					emitter( )->emit( "listener_removed", std::move( event ), std::move( listener ) );
				}
			};	// class StandardEvents

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
