#pragma once


#include "base_types.h"
#include "lib_net_server.h"
#include "lib_net_socket_stream.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				using namespace daw::nodepp;

				NetServer create_server( base::options_t options = base::options_t{ } );
				template<typename Listener>
				NetServer create_server( base::options_t options, Listener listener ) {
					return create_server( options ).on( "connection", listener );
					
				}

				NetSocketStream create_connection( base::options_t options );
				template<typename Listener>
				NetServer create_connection( base::options_t options, Listener listener ) {
					return create_connection( options ).on( "connect", listener );

				}

				NetSocketStream create_connection( uint16_t port, std::string host = "" );
				
				template<typename Listener>
				NetServer create_connection( uint16_t port, std::string host = "", Listener listener ) {
					return create_connection( options ).on( "connect", std::move( port ), std::move( host ) );

				}

				uint8_t is_ip( std::string ip_address );
				bool is_ipv4( std::string ip_address );
				bool is_ipv6( std::string ip_address );
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
