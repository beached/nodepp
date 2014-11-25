#pragma once


#include "base_types.h"
#include "lib_net_server.h"
#include "lib_net_socket.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				NetServer create_server( options_t options = options_t{ }, NetServer::events_t::callback_t_connection callback = NetServer::events_t::callback_t_connection{ } );
				NetSocket create_connection( options_t options, NetSocket::events_t::callback_t_connect callback = NetSocket::events_t::callback_t_connect{ } );
				NetSocket create_connection( uint16_t port, std::string host = "", NetSocket::events_t::callback_t_connect callback = NetSocket::events_t::callback_t_connect{ } );

				uint8_t is_ip( std::string ip_address );
				bool is_ipv4( std::string ip_address );
				bool is_ipv6( std::string ip_address );
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
