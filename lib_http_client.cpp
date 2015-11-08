// The MIT License (MIT)
//
// Copyright (c) 2014-2015 Darrell Wright
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

#include <cstdlib>
#include <memory>
#include <iostream>

#include "lib_http_client.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				HttpClient create_http_client( daw::nodepp::base::EventEmitter emitter ) {
					// TODO
					return HttpClient { nullptr };
				}

				HttpClientConnection create_http_client_connection( daw::nodepp::lib::net::NetSocketStream socket, daw::nodepp::base::EventEmitter emitter ) {
					return std::make_shared<impl::HttpClientConnectionImpl>( std::move( socket ), std::move( emitter ) );
				}

				namespace impl {
					HttpClientImpl & HttpClientImpl::on_connection( std::function<void( HttpClientConnection )> listener ) {
						return *this;
					}

					void HttpClientImpl::request( std::string scheme, std::string host, uint16_t port, daw::nodepp::lib::http::HttpClientRequest request ) {
						auto socket = m_client;
						socket->on_connected( [socket, scheme, request, host, port]( ) {
							auto const & request_line = request->request_line;
							socket->write_async( to_string( request_line.method ) + " " + to_string( request_line.url ) + " HTTP/1.1\r\n" );
							socket->write_async( "Host: " + host + ":" + std::to_string( port ) + "\r\n\r\n" );
							socket->set_read_mode( net::NetSocketStreamReadMode::newline );
							socket->read_async( );
						} ).on_data_received( [socket]( std::shared_ptr<base::data_t> data_buffer, bool ) {
							std::string buf { data_buffer->begin( ), data_buffer->end( ) };
							std::cout << buf << std::endl;
						} );

						socket->connect( host, port );
					}

					HttpClientConnectionImpl::HttpClientConnectionImpl( daw::nodepp::lib::net::NetSocketStream socket, daw::nodepp::base::EventEmitter emitter ): m_socket( socket ), m_emitter( emitter ) { }

					HttpClientConnectionImpl& HttpClientConnectionImpl::on_response_returned( std::function<void( daw::nodepp::lib::http::HttpServerResponse )> listener ) {
						return *this;
					}

					HttpClientConnectionImpl& HttpClientConnectionImpl::on_next_response_returned( std::function<void( daw::nodepp::lib::http::HttpServerResponse )> listener ) {
						return *this;
					}

					HttpClientConnectionImpl & HttpClientConnectionImpl::on_closed( std::function<void( )> listener ) {
						return *this;
					}
				}	//namespace impl
			}	// namespace http
		} // namespace lib
	}	// namespace nodepp
}	// namespace daw