// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <boost/utility/string_ref.hpp>
#include <boost/asio/error.hpp>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

#include "base_callback.h"
#include "base_error.h"
#include <daw/daw_range_algorithm.h>

namespace daw {
	namespace nodepp {
		namespace base {
			template<typename Derived>
			struct enable_shared: public std::enable_shared_from_this <Derived> {
				std::shared_ptr<Derived> get_ptr( ) { return static_cast<Derived*>(this)->shared_from_this( ); }
				std::weak_ptr<Derived> get_weak_ptr( ) { return this->get_ptr( ); }
			protected:
				~enable_shared( ) = default;
			};	// struct enable_shared

			namespace impl {
				struct EventEmitterImpl;
			}	// namespace impl

			using EventEmitter = ::std::shared_ptr <impl::EventEmitterImpl>;

			EventEmitter create_event_emitter( );

			namespace impl {
				//////////////////////////////////////////////////////////////////////////
				/// Summary:	Allows for the dispatch of events to subscribed listeners
				///				Callbacks can be be c-style function pointers, lambda's or
				///				std::function with the correct signature.
				///	Requires:	base::Callback
				///
				struct EventEmitterImpl final {
					using listener_list_t = std::vector <std::pair<bool, Callback>>;
					using listeners_t = std::unordered_map <std::string, listener_list_t>;
					using callback_id_t = Callback::id_t;
				private:
					const int_least8_t c_max_emit_depth = 100;	// TODO: Magic Number
					std::shared_ptr<listeners_t> m_listeners;
					size_t m_max_listeners;
					std::shared_ptr<std::atomic_int_least8_t> m_emit_depth;
					bool m_allow_cb_without_params;

					EventEmitterImpl( size_t max_listeners = 10 );
				public:
					friend base::EventEmitter base::create_event_emitter( );

					EventEmitterImpl( EventEmitterImpl const & ) = delete;
					~EventEmitterImpl( ) = default;
					EventEmitterImpl& operator=( EventEmitterImpl const & ) = delete;
					EventEmitterImpl( EventEmitterImpl && ) = delete;
					EventEmitterImpl& operator=( EventEmitterImpl && ) = delete;

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
							listeners( )[event.to_string( )].emplace_back( run_once, callback );
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

				private:
					template<typename... Args>
					void emit_impl( boost::string_ref event, Args&&... args ) {
						auto& callbacks = listeners( )[event.to_string( )];
						for( auto& callback : callbacks ) {
							if( !callback.second.empty( ) ) {
								try {
									callback.second( std::forward<Args>( args )... );
								} catch( boost::bad_any_cast const & ) {
									if( m_allow_cb_without_params ) {
										try {
											callback.second( );
										} catch( boost::bad_any_cast const & ) {
											throw std::runtime_error( "Type of event listener does not match.  This shouldn't happen" );
										}
									} else {
										throw;
									}
								}
							}
						}
						daw::algorithm::erase_remove_if( callbacks, []( std::pair<bool, Callback> const & item ) {
							return item.first;
						} );
						--(*m_emit_depth);
					}

				public:
					template<typename... Args>
					void emit( boost::string_ref event, Args&&... args ) {
						if( event.empty( ) ) {
							throw std::runtime_error( "Empty event name passed to emit" );
						}

						if( ++(*m_emit_depth)> c_max_emit_depth ) {
							throw std::runtime_error( "Max callback depth reached.  Possible loop" );
						}
						emit_impl( event, std::forward<Args>( args )... );
						auto const event_selfdestruct = event.to_string( ) + "_selfdestruct";
						if( listeners( ).count( event_selfdestruct ) ) {
							emit_impl( event_selfdestruct );	// Called by self destruct code and must be last so lifetime is controlled
						}
					}

					void emit_listener_added( boost::string_ref event, Callback listener );
					void emit_listener_removed( boost::string_ref event, Callback listener );

					bool at_max_listeners( boost::string_ref event );
				};	// class EventEmitterImpl
			}	// namespace impl

			//////////////////////////////////////////////////////////////////////////
			// Allows one to have the Events defined in event emitter
			template<typename Derived>
			class StandardEvents {
				daw::nodepp::base::EventEmitter m_emitter;

				Derived& child( ) {
					return *static_cast<Derived*>(this);
				}

				void emit_error( base::Error error ) {
					m_emitter->emit( "error", std::move( error ) );
				}

			public:
				StandardEvents( ) = delete;
				explicit StandardEvents( daw::nodepp::base::EventEmitter emitter ): m_emitter( std::move( emitter ) ) { }
				virtual ~StandardEvents( ) = default;
				StandardEvents( StandardEvents const & ) = default;
				StandardEvents( StandardEvents && ) = default;
				StandardEvents & operator=( StandardEvents const & ) = default;
				StandardEvents & operator=( StandardEvents && ) = default;

				EventEmitter& emitter( ) {
					return m_emitter;	// If you get a warning about a recursive call here, you forgot to create a emitter() in Derived
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Callback is for when error's occur
				Derived& on_error( std::function<void( base::Error )> listener ) {
					m_emitter->add_listener( "error", listener );
					return child( );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Callback is for the next error
				Derived& on_next_error( std::function<void( base::Error )> listener ) {
					m_emitter->add_listener( "error", listener, true );
					return child( );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary:	Callback is called whenever a new listener is added for
				///				any callback
				Derived& on_listener_added( std::function<void( std::string, Callback )> listener ) {
					m_emitter->add_listener( "listener_added", listener );
					return child( );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary:	Callback is called when the next new listener is added
				///				for any callback
				Derived& on_next_listener_added( std::function<void( std::string, Callback )> listener ) {
					m_emitter->add_listener( "listener_added", listener, true );
					return child( );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Callback is called whenever a listener is removed for
				/// any callback
				Derived& on_listener_removed( std::function<void( std::string, Callback )> listener ) {
					m_emitter->add_listener( "listener_removed", listener );
					return child( );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Callback is called the next time a listener is removed for
				/// any callback
				Derived& on_next_listener_removed( std::function<void( std::string, Callback )> listener ) {
					m_emitter->add_listener( "listener_removed", listener, true );
					return child( );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary:	Callback is called when the subscribed object is exiting.
				///				This does not necessarily, but can be, from it's
				///				destructor.  Make sure to wrap in try/catch if in
				///				destructor
				Derived& on_exit( std::function<void( OptionalError error )> listener ) {
					m_emitter->add_listener( "exit", listener );
					return child( );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary:	Callback is called when the subscribed object is exiting.
				///				This does not necessarily, but can be, from it's
				///				destructor.  Make sure to wrap in try/catch if in
				///				destructor
				Derived& on_next_exit( std::function<void( OptionalError error )> listener ) {
					m_emitter->add_listener( "exit", listener, true );
					return child( );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Delegate error callbacks to another error handler
				template<typename StandardEventsChild>
				Derived& on_error( std::weak_ptr<StandardEventsChild> error_destination, std::string where ) {
					on_error( [error_destination, where]( base::Error error ) mutable {
						if( !error_destination.expired( ) ) {
							error_destination.lock( )->emit_error( std::move( error ), where );
						}
					} );
					return child( );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Delegate error callbacks to another error handler
				template<typename StandardEventsChild>
				Derived& on_error( std::shared_ptr<StandardEventsChild> error_destination, std::string where ) {
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
				void emit_error( base::Error child, boost::string_ref where ) {
					base::Error err( "Derived Error" );
					err.add( "where", where.to_string( ) );
					err.child( std::move( child ) );

					emit_error( std::move( err ) );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Emit an error event
				void emit_error( ErrorCode const & error, boost::string_ref where ) {
					base::Error err( error );
					err.add( "where", where.to_string( ) );

					emit_error( err );
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

				//////////////////////////////////////////////////////////////////////////
				/// Summary:	Emit and event when exiting to alert others that they
				///				may want to stop and exit. This version allows for an
				///				error reason
				void emit_exit( Error error ) {
					m_emitter->emit( "exit", create_optional_error( std::move( error ) ) );
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary:	Emit and event when exiting to alert others that they
				///				may want to stop and exit.
				void emit_exit( ) {
					m_emitter->emit( "exit", create_optional_error( ) );
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
						auto mself = daw::as_move_capture( daw::copy( self ) );
						auto mfunc = daw::as_move_capture( std::move( func ) );
						emit_error_on_throw( self, err_description, where, [mfunc, mself]( ) mutable {
							mfunc.value( )(mself.move_out( ));
						} );
					}
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary:	Creates a callback on the event source that calls a
				///				mirroring emitter on the destination obj. Unless the
				///				callbacks are of the form std::function<void( )> the
				///				callback parameters must be template parameters here.
				///				e.g.
				///				obj_emitter.delegate_to<daw::nodepp::lib::net::EndPoint>( "listening", dest_obj.get_weak_ptr( ), "listening" );
				template<typename... Args, typename DestinationType>
				Derived& delegate_to( boost::string_ref source_event, std::weak_ptr<DestinationType> destination_obj, std::string destination_event ) {
					auto handler = [destination_obj, destination_event]( Args&&... args ) {
						if( !destination_obj.expired( ) ) {
							destination_obj.lock( )->emitter( )->emit( destination_event, std::forward<Args>( args )... );
						}
					};
					m_emitter->on( source_event, handler );
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

