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

#include <memory>

#include "lib_net_socket_stream.h"
#include "lib_http_request.h"
#include "lib_http_server_response.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				enum class HttpConnectionState { Request, Message };

				namespace impl { class HttpServerConnectionImpl; }
				using HttpServerConnection = std::shared_ptr <impl::HttpServerConnectionImpl>;
				HttpServerConnection create_http_server_connection( daw::nodepp::lib::net::NetSocketStream&& socket, daw::nodepp::base::EventEmitter emitter = daw::nodepp::base::create_event_emitter( ) );

				namespace impl {
					class HttpServerConnectionImpl final: public daw::nodepp::base::enable_shared<HttpServerConnectionImpl>, public daw::nodepp::base::StandardEvents <HttpServerConnectionImpl> {
						daw::nodepp::lib::net::NetSocketStream m_socket;

						HttpServerConnectionImpl( daw::nodepp::lib::net::NetSocketStream && socket, daw::nodepp::base::EventEmitter emitter );
					public:
						friend HttpServerConnection daw::nodepp::lib::http::create_http_server_connection( daw::nodepp::lib::net::NetSocketStream&&, daw::nodepp::base::EventEmitter emitter );

						HttpServerConnectionImpl( ) = delete;

						HttpServerConnectionImpl( HttpServerConnectionImpl const & ) = delete;
						HttpServerConnectionImpl& operator=( HttpServerConnectionImpl const & ) = delete;
						HttpServerConnectionImpl( HttpServerConnectionImpl && ) = default;
						HttpServerConnectionImpl& operator=( HttpServerConnectionImpl && ) = default;

						~HttpServerConnectionImpl( );

						HttpServerConnectionImpl& on_client_error( std::function<void( daw::nodepp::base::Error )> listener );
						HttpServerConnectionImpl& on_next_client_error( std::function<void( daw::nodepp::base::Error )> listener );

						HttpServerConnectionImpl& on_request_made( std::function<void( HttpClientRequest, HttpServerResponse )> listener );
						HttpServerConnectionImpl& on_next_request_made( std::function<void( HttpClientRequest, HttpServerResponse )> listener );

						HttpServerConnectionImpl& on_closed( std::function<void( )> listener );	// Only once as it is called on the way out
						void close( );
						void start( );
						daw::nodepp::lib::net::NetSocketStream socket( );

						void emit_closed( );
						void emit_client_error( daw::nodepp::base::Error error );
						void emit_request_made( HttpClientRequest request, HttpServerResponse response );
					};	// class HttpConnectionImpl
				}	// namespace impl
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
