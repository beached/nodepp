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

#include <cstdlib>
#include <memory>
#include <iostream>

#include "lib_http_client.h"
#include "lib_http_request.h"
#include "lib_http_url.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				HttpClient create_http_client( daw::nodepp::base::EventEmitter emitter ) {
					return std::make_shared<impl::HttpClientImpl>( std::move( emitter ) );
				}

				HttpClientConnection create_http_client_connection( daw::nodepp::lib::net::NetSocketStream socket, daw::nodepp::base::EventEmitter emitter ) {
					return std::make_shared<impl::HttpClientConnectionImpl>( std::move( socket ), std::move( emitter ) );
				}

				namespace impl {
					HttpClientImpl::HttpClientImpl( daw::nodepp::base::EventEmitter emitter ):
						daw::nodepp::base::StandardEvents<HttpClientImpl>( std::move( emitter ) ),
						m_client( net::create_net_socket_stream( ) ) { }

					HttpClientImpl & HttpClientImpl::on_connection( std::function<void( HttpClientConnection )> listener ) {
						return *this;
					}
				
					HttpClientImpl::~HttpClientImpl( ) { }
					HttpClientConnectionImpl::~HttpClientConnectionImpl( ) { }
					void HttpClientImpl::request( std::string scheme, std::string host, uint16_t port, daw::nodepp::lib::http::HttpClientRequest request ) {
						auto socket = m_client;
						socket->on_connected( [scheme, request, host, port]( auto s ) mutable {
							auto const & request_line = request->request_line;
							std::stringstream ss;
							ss <<to_string( request_line.method ) <<" " <<to_string( request_line.url ) <<" HTTP/1.1\r\n";
							ss <<"Host: " <<host <<":" <<std::to_string( port ) <<"\r\n\r\n";
							auto msg = ss.str( );
							s->end( msg );
							s->set_read_mode( net::NetSocketStreamReadMode::double_newline );
							s->read_async( );
						} ).on_data_received( []( std::shared_ptr<base::data_t> data_buffer, bool ) {
							if( data_buffer ) {
								for( auto const & ch : *data_buffer ) {
									std::cout <<ch;
								}
								std::cout <<std::endl;
							}
						} );

						socket->connect( host, port );
					}

					HttpClientConnectionImpl::HttpClientConnectionImpl( daw::nodepp::lib::net::NetSocketStream socket, daw::nodepp::base::EventEmitter emitter ):
						daw::nodepp::base::StandardEvents<HttpClientConnectionImpl>( std::move( emitter ) ),
						m_socket( std::move( socket ) ) { }

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

				// TODO: should be returning a response
				void get( boost::string_ref url_string, std::initializer_list<std::pair<std::string, HttpClientConnectionOptions::value_type>> options, std::function<void( HttpClientResponseMessage )> on_completion ) {				
					auto url = parse_url( url_string );
					std::cout <<"url: " <<url->encode( ) <<std::endl;
					std::cout <<"url: " <<url <<std::endl;
				}
			}	// namespace http
		} // namespace lib
	}	// namespace nodepp
}	// namespace daw

