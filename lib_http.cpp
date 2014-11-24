#include <boost/shared_ptr.hpp>
#include <cinttypes>
#include <string>
#include <utility>

#include "lib_http.h"
#include "lib_net_server.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using events_t = Server::events_t;

				class ServerImpl {
				public:
					events_t events;

					ServerImpl( ) : events{ } { }

					ServerImpl( events_t::callback_t_request func ) : events{ } {
						on( events_t::types::request, func );
					}

					~ServerImpl( ) { }

					ServerImpl( ServerImpl&& other ) = delete;
					ServerImpl& operator=(ServerImpl&& rhs) = delete;
					ServerImpl( ServerImpl const & ) = delete;
					ServerImpl& operator=(ServerImpl const &) = delete;
					
					void listen( uint16_t port, std::string hostname, uint16_t backlog, events_t::callback_t_listening func ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					void listen( std::string path, events_t::callback_t_listening func ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					void listen( daw::nodepp::lib::net::Handle& handle, events_t::callback_t_listening func ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					void on( events_t::types et, events_t::callback_t_request func ) {	// request, check_continue event(s) 					
						switch( et ) {
						case events_t::types::request:
							events.request.on( events_t::event_t_request::callback_t( func ) );
							break;
						case events_t::types::check_continue:
							events.check_continue.on( events_t::event_t_check_continue::callback_t( func ) );
							break;
						default:
							throw std::runtime_error( "Unexpected Server event type passed" );
						}
					}

					void once( events_t::types et, events_t::callback_t_request func ) {	// request, check_continue event(s)
						switch( et ) {
						case events_t::types::request:
							events.request.once( events_t::event_t_request::callback_t( func ) );
							break;
						case events_t::types::check_continue:
							events.check_continue.once( events_t::event_t_check_continue::callback_t( func ) );
							break;
						default:
							throw std::runtime_error( "Unexpected Server event type passed" );
						}
					}

					void on( events_t::types et, events_t::callback_t_connection func ) {	// connection event(s)
						switch( et ) {
						case events_t::types::connection:
							events.connection.on( events_t::event_t_connection::callback_t( func ) );
							break;
						default:
							throw std::runtime_error( "Unexpected Server event type passed" );
						}
					}

					void once( events_t::types et, events_t::callback_t_connection func ) {	// connection event(s)
						switch( et ) {
						case events_t::types::connection:
							events.connection.once( events_t::event_t_connection::callback_t( func ) );
							break;
						default:
							throw std::runtime_error( "Unexpected Server event type passed" );
						}
					}

					void on( events_t::types et, events_t::callback_t_close func ) {	// close event(s)
						switch( et ) {
						case events_t::types::close:
							events.close.on( events_t::event_t_close::callback_t( func ) );
							break;
						default:
							throw std::runtime_error( "Unexpected Server event type passed" );
						}
					}

					void once( events_t::types et, events_t::callback_t_close func ) {	// close event(s)
						switch( et ) {
						case events_t::types::close:
							events.close.once( events_t::event_t_close::callback_t( func ) );
							break;
						default:
							throw std::runtime_error( "Unexpected Server event type passed" );
						}
					}

					void on( events_t::types et, events_t::callback_t_connect func ) {	// connect, upgrade event(s)
						switch( et ) {
						case events_t::types::connect:
							events.connect.on( events_t::event_t_connect::callback_t( func ) );
							break;
						case events_t::types::upgrade:
							events.upgrade.on( events_t::event_t_upgrade::callback_t( func ) );
							break;
						default:
							throw std::runtime_error( "Unexpected Server event type passed" );
						}
					}

					void once( events_t::types et, events_t::callback_t_connect func ) {	// connect, upgrade event(s)
						switch( et ) {
						case events_t::types::connect:
							events.connect.once( events_t::event_t_connect::callback_t( func ) );
							break;
						case events_t::types::upgrade:
							events.upgrade.once( events_t::event_t_upgrade::callback_t( func ) );
							break;
						default:
							throw std::runtime_error( "Unexpected Server event type passed" );
						}
					}

					void on( events_t::types et, events_t::callback_t_client_error func ) {	// client_error event(s)
						// TODO
						switch( et ) {
						case events_t::types::client_error:
							events.client_error.on( events_t::event_t_client_error::callback_t{ func } );
							break;
						default:
							throw std::runtime_error( "Unexpected Server event type passed" );
						}
					}

					void once( events_t::types et, events_t::callback_t_client_error func ) {	// client_error event(s)
						switch( et ) {
						case events_t::types::client_error:
							events.client_error.once( events_t::event_t_client_error::callback_t{ func } );
							break;
						default:
							throw std::runtime_error( "Unexpected Server event type passed" );
						}
					}

					size_t& max_header_count( ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					size_t const & max_header_count( ) const {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					void set_timeout( size_t msecs, daw::nodepp::lib::net::Socket::events_t::callback_t_timeout callback ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					size_t timeout( ) const {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

				};	// class ServerImpl


				


				Server create_server( events_t::callback_t_request func ) {
					return Server( func );
				}
			}
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
