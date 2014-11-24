#pragma once

#include "lib_net_server.h"
#include "lib_net_socket.h"
#include "lib_types.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				Server create_server( options_t options = options_t{ }, Server::events_t::callback_t_connection callback = Server::events_t::callback_t_connection{ } );
				Socket create_connection( options_t options, Socket::events_t::callback_t_connect callback = Socket::events_t::callback_t_connect{ } );
				Socket create_connection( uint16_t port, std::string host = "", Socket::events_t::callback_t_connect callback = Socket::events_t::callback_t_connect{ } );

				uint8_t is_ip( std::string ip_address );
				bool is_ipv4( std::string ip_address );
				bool is_ipv6( std::string ip_address );
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
