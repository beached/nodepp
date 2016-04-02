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

#include "base_types.h"
#include "lib_net_server.h"
#include "lib_net_socket_stream.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				NetServer create_server( daw::nodepp::base::options_t options = daw::nodepp::base::options_t { } );
				template<typename Listener>
				NetServer create_server( daw::nodepp::base::options_t options, Listener listener ) {
					return create_server( options )->on( "connection", listener );
				}

				NetSocketStream create_connection( daw::nodepp::base::options_t options );
				template<typename Listener>
				NetServer create_connection( daw::nodepp::base::options_t options, Listener listener ) {
					return create_connection( options )->on( "connect", listener );
				}

				NetSocketStream create_connection( uint16_t port, std::string host = "" );

				template<typename Listener>
				NetServer create_connection( uint16_t port, std::string host = "", Listener listener ) {
					return create_connection( port, host )->on( "connect", listener );
				}

				uint8_t is_ip( std::string ip_address );
				bool is_ipv4( std::string ip_address );
				bool is_ipv6( std::string ip_address );
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw

