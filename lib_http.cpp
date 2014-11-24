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

				Server::Server( ) : events{ } { }

				Server::Server( events_t::callback_t_request func ) : events{ std::make_shared<events_t>( ) } {
					on( events_t::types::request, func );
				}

				Server::Server( Server&& other ) : events( std::move( other.events ) ) { }

				Server& Server::operator=(Server&& rhs) {
					if( this != &rhs ) {
						events = std::move( rhs.events );
						//rhs.events.clear( );
					}
					return *this;
				}

				Server::~Server( ) { }

				Server& Server::on( events_t::types et, events_t::callback_t_request func ) {	// request, check_continue event(s) 					
					switch( et ) {
					case events_t::types::request:
						events->request.on( events_t::event_t_request::callback_t( func ) );
						break;
					case events_t::types::check_continue:
						events->check_continue.on( events_t::event_t_check_continue::callback_t( func ) );
						break;
					default:
						throw std::runtime_error( "Unexpected Server event type passed" );
					}
					return *this;
				}

				Server& Server::once( events_t::types et, events_t::callback_t_request func ) {	// request, check_continue event(s)
					switch( et ) {
					case events_t::types::request:
						events->request.once( events_t::event_t_request::callback_t( func ) );
						break;
					case events_t::types::check_continue:
						events->check_continue.once( events_t::event_t_check_continue::callback_t( func ) );
						break;
					default:
						throw std::runtime_error( "Unexpected Server event type passed" );
					}
					return *this;
				}


				Server create_server( std::function<void( Request, Response )> func ) {
					return Server( func );
				}
			}
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
