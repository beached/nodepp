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

#include <list>
#include <memory>
#include <vector>

#include "base_event_emitter.h"
#include "lib_http_connection.h"
#include "lib_http_server_response.h"
#include "lib_net_server.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				namespace impl {
					class HttpServerImpl;
				}
				using HttpServer = std::shared_ptr <impl::HttpServerImpl>;

				HttpServer create_http_server( daw::nodepp::base::EventEmitter emitter = daw::nodepp::base::create_event_emitter( ) );

				namespace impl {
					//////////////////////////////////////////////////////////////////////////
					// Summary:		An HTTP Server class
					// Requires:	daw::nodepp::lib::net::NetServer
					class HttpServerImpl final: public daw::nodepp::base::enable_shared<HttpServerImpl>, public daw::nodepp::base::StandardEvents <HttpServerImpl> {
						daw::nodepp::lib::net::NetServer m_netserver;
						std::list<HttpServerConnection> m_connections;

						static void handle_connection( std::weak_ptr<HttpServerImpl> obj, daw::nodepp::lib::net::NetSocketStream socket );

						explicit HttpServerImpl( daw::nodepp::base::EventEmitter emitter );
					public:
						friend daw::nodepp::lib::http::HttpServer daw::nodepp::lib::http::create_http_server( daw::nodepp::base::EventEmitter );

						~HttpServerImpl( );
						HttpServerImpl( HttpServerImpl const & ) = default;
						HttpServerImpl( HttpServerImpl &&  ) = default;
						HttpServerImpl& operator=( HttpServerImpl const & ) = default;
						HttpServerImpl& operator=( HttpServerImpl && ) = default;

						void listen_on( uint16_t port );

						void listen_on( uint16_t port, std::string hostname, uint16_t backlog = 511 );

						template<typename Listener>
						void listen_on( uint16_t port, std::string hostname, uint16_t backlog, Listener listener ) {
							daw::nodepp::base::rollback_event_on_exception( this, "listening", listener, [&]( ) {
								listen_on( port, hostname, backlog );
							} );
						}

						void listen_on( std::string path );

						template<typename Listener>
						void listen_on( std::string path, Listener listener ) {
							daw::nodepp::base::rollback_event_on_exception( this, "listening", listener, [&]( ) {
								listen_on( path );
							} );
						}

						void listen_on( daw::nodepp::base::ServiceHandle handle );

						template<typename Listener>
						void listen_on( daw::nodepp::base::ServiceHandle handle, Listener listener ) {
							daw::nodepp::base::rollback_event_on_exception( this, "listening", listener, [&]( ) {
								listen_on( std::move( handle ) );
							} );
						}

						size_t& max_header_count( );
						size_t const & max_header_count( ) const;

						template<typename Listener>
						void set_timeout( size_t msecs, Listener listener ) {
							throw std::runtime_error( "Method not implemented" );
						}

						HttpServerImpl& on_listening( std::function<void( daw::nodepp::lib::net::EndPoint )> listener );
						HttpServerImpl& on_next_listening( std::function<void( daw::nodepp::lib::net::EndPoint )> listener );

						HttpServerImpl& on_client_connected( std::function<void( HttpServerConnection )> listener );
						HttpServerImpl& on_next_client_connected( std::function<void( HttpServerConnection )> listener );

						HttpServerImpl& on_closed( std::function<void( )> listener );
						HttpServerImpl& on_next_closed( std::function<void( )> listener );

						size_t timeout( ) const;

						void emit_client_connected( HttpServerConnection connection );
						void emit_closed( );
						void emit_listening( daw::nodepp::lib::net::EndPoint endpoint );
					};	// class Server
				}	// namespace impl
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw

