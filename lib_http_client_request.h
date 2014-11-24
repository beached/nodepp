#pragma once

#include <string>

#include "lib_event.h"


namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				class Request { };
				class IncomingMessage { };
				class Header;
				class ClientRequest {
				public:
					struct events_t {
						CREATE_EVENT( response, IncomingMessage );
						CREATE_EVENT( socket, daw::nodepp::lib::net::Socket );
						CREATE_EVENT( connect, IncomingMessage, daw::nodepp::lib::net::Socket, Header );
						CREATE_EVENT( upgrade, IncomingMessage, daw::nodepp::lib::net::Socket, Header );
						CREATE_EVENT( continued );
						enum class types { response, socket, connect, upgrade, continued };
					} events;

					ClientRequest& on( events_t::types event_type, events_t::callback_t_response callback );
					ClientRequest& once( events_t::types event_type, events_t::callback_t_response callback );

					ClientRequest& on( events_t::types event_type, events_t::callback_t_socket callback );
					ClientRequest& once( events_t::types event_type, events_t::callback_t_socket callback );

					ClientRequest& on( events_t::types event_type, events_t::callback_t_connect callback );
					ClientRequest& once( events_t::types event_type, events_t::callback_t_connect callback );

					ClientRequest& on( events_t::types event_type, events_t::callback_t_continued callback );
					ClientRequest& once( events_t::types event_type, events_t::callback_t_continued callback );

					bool write( std::string chunk, daw::nodepp::lib::encoding_t encoding = "" );
					bool write( daw::nodepp::lib::net::Socket::data_t chunk );

					void end( );
					void end( std::string data, daw::nodepp::lib::encoding_t encoding = "" );
					void end( daw::nodepp::lib::net::Socket::data_t data );
					void abort( );
				};

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
