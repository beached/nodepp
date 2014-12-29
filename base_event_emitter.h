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

			template<typename T>
			struct enable_shared: public std::enable_shared_from_this< T > {
				std::shared_ptr<T> get_ptr( ) { return static_cast<T*>(this)->shared_from_this( ); }
				std::weak_ptr<T> get_weak_ptr( ) { return get_ptr( ); }
			};

			namespace impl {
				//////////////////////////////////////////////////////////////////////////
				/// Summary:	Allows for the dispatch of events to subscribed listeners
				///				Callbacks can be be c-style function pointers, lambda's or 
				///				std::function with the correct signature.
				///	Requires:	base::Callback
				///	
				struct EventEmitterImpl: enable_shared< EventEmitterImpl > {
					using listener_list_t = std::vector < std::pair<bool, Callback> > ;
					using listeners_t = std::unordered_map < std::string, listener_list_t > ;
					using callback_id_t = Callback::id_t;
				private:
					const int_least8_t c_max_emit_depth = 100;	// TODO Magic Number
					std::shared_ptr<std::unordered_map<std::string, listener_list_t>> m_listeners;
					size_t m_max_listeners;
					std::shared_ptr<std::atomic_int_least8_t> m_emit_depth;
					EventEmitterImpl( );
				public:
					friend base::EventEmitter base::create_event_emitter( );

					~EventEmitterImpl( ) = default;
					EventEmitterImpl( EventEmitterImpl const & ) = delete;
					EventEmitterImpl& operator=(EventEmitterImpl const &) = delete;
					EventEmitterImpl( EventEmitterImpl && other ) = delete;
					EventEmitterImpl& operator=(EventEmitterImpl && rhs) = delete;

					void remove_listener( boost::string_ref event, callback_id_t id );

					void remove_listener( boost::string_ref event, Callback listener );

					void remove_all_listeners( );

					void remove_all_listeners( boost::string_ref event );

					void set_max_listeners( size_t max_listeners );
					listeners_t & listeners( );
					listener_list_t listeners( boost::string_ref event );
					size_t listener_count( boost::string_ref event );

					template<typename Listener>
					callback_id_t add_listener( boost::string_ref event, Listener listener, bool run_once = false ) {
						if( event.empty( ) ) {
							throw std::runtime_error( "Empty event name passed to add_listener" );
						}
						if( !at_max_listeners( event ) ) {
							auto callback = Callback( listener );
							if( event != "newListener" ) {
								emit_listener_added( event, callback );
							}
							listeners( )[event.to_string()].emplace_back( run_once, callback );
							return callback.id( );
						} else {
							// TODO: implement logging to fail gracefully.  For now throw
							throw std::runtime_error( "Max listeners reached for event" );
						}
					}

					template<typename Listener>
					void on( boost::string_ref event, Listener listener ) {
						add_listener( event, listener );
					}

					template<typename Listener>
					void on_next( boost::string_ref event, Listener listener ) {
						add_listener( event, listener, true );
					}

					template<typename... Args>
					void emit( std::string event, Args&&... args ) {						
						if( event.empty( ) ) {
							throw std::runtime_error( "Empty event name passed to emit" );
						}

						if( ++(*m_emit_depth) > c_max_emit_depth ) {
							throw std::runtime_error( "Max callback depth reached.  Possible loop" );
						}

						auto& callbacks = listeners( )[event];
						for( auto& callback : callbacks ) {
							if( !callback.second.empty( ) ) {
								callback.second.exec( std::move( args )... );
							}
						}

						daw::algorithm::erase_remove_if( callbacks, []( std::pair<bool, Callback> const & item ) {
							return item.first;
						} );
						--(*m_emit_depth);
					}

					void emit_listener_added( boost::string_ref event, Callback listener );
					void emit_listener_removed( boost::string_ref event, Callback listener );

					bool at_max_listeners( boost::string_ref event );
				};	// class EventEmitterImpl
			}	// namespace impl
			
			//////////////////////////////////////////////////////////////////////////
			// Allows one to have the Events defined in event emitter
			template<typename Child>
			class StandardEvents {
				Child& child( ) {
					return *static_cast<Child*>(this);
				}				

				void emit_error( base::Error error ) {
					emitter( )->emit( "error", std::move( error ) );
				}
			public:
				EventEmitter& emitter( ) {
					return child( ).emitter( );	// If you get a warning about a recursive call here, you forgot to create a emitter() in Child
				}

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

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Delegate error callbacks to another error handler
				template<typename StandardEventsChild>
				Child& on_error( std::weak_ptr<StandardEventsChild> error_destination, std::string where ) {
					on_error( [error_destination, where]( base::Error error ) mutable {
						if( !error_destination.expired( ) ) {
							error_destination.lock( )->emit_error( where, std::move( error ) );
						}
					} );
					return child( );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Delegate error callbacks to another error handler
				template<typename StandardEventsChild>
				Child& on_error( std::shared_ptr<StandardEventsChild> error_destination, std::string where ) {
					return on_error( std::weak_ptr<StandardEventsChild>( error_destination ), std::move( where ) );
				}
			
				//////////////////////////////////////////////////////////////////////////
				/// Summary: Emit an error event
				void emit_error( boost::string_ref description, boost::string_ref where ) {
					base::Error err( description );
					err.add( "where", where );

					emit_error( std::move( err ) );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Emit an error event
				void emit_error( boost::string_ref where, base::Error child ) {
					base::Error err( "Child Error" );
					err.add( "where", where.to_string() );
					err.child( std::move( child ) );

					emit_error( std::move( err ) );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Emit an error event
				void emit_error( boost::system::error_code const & error, boost::string_ref where ) {
					base::Error err( error );
					err.add( "where", where.to_string() );

					emit_error( std::move( err ) );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Emit an error event
				void emit_error( std::exception_ptr ex, boost::string_ref description, boost::string_ref where ) {
					base::Error err( description, ex );
					err.add( "where", where );

					emit_error( std::move( err ) );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary:	Emit an event with the callback and event name of a newly
				///				added event
				void emit_listener_added( boost::string_ref event, Callback listener ) {
					emitter( )->emit_listener_added( event, listener );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary:	Emit an event with the callback and event name of an event
				///				that has been removed
				void emit_listener_removed( boost::string_ref event, Callback listener ) {
					emitter( )->emit( "listener_removed", event, std::move( listener ) );
				}

				template<typename Class, typename Func>
				static void emit_error_on_throw( std::shared_ptr<Class> self, boost::string_ref err_description, boost::string_ref where, Func func ) {
					try {
						func( );
					} catch( ... ) {
						self->emit_error( std::current_exception( ), err_description, where );
					}
				}

				template<typename Class, typename Func>
				static void run_if_valid( std::weak_ptr<Class> obj, boost::string_ref err_description, boost::string_ref where, Func func ) {
					if( !obj.expired( ) ) {
						auto self = obj.lock( );
						emit_error_on_throw( self, err_description, where, [&]( ) {
							func( self );
						} );
					}
				}

				template<typename... Args, typename DestinationType>
				Child& delegate_to( boost::string_ref source_event, std::weak_ptr<DestinationType> destination_obj, std::string destination_event ) {
					auto handler = [destination_obj, destination_event]( Args... args ) {
						if( !destination_obj.expired( ) ) {
							destination_obj.lock( )->emitter( )->emit( destination_event, std::move( args )... );
						}
					};
					emitter( )->on( source_event, handler );
					return child( );
				}

			};	// class StandardEvents			

			template<typename This, typename Listener, typename Action>
			static auto rollback_event_on_exception( This me, boost::string_ref event, Listener listener, Action action_to_try, bool run_listener_once = false ) -> decltype(action_to_try( )) {
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
