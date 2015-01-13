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

#include <regex>
#include <boost/regex.hpp>
#include <memory>

#include "lib_http_connection.h"
#include "lib_http_request.h"
#include "lib_net_socket_stream.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				namespace impl {
					namespace {
						void err400( lib::net::NetSocketStream& socket ) {
							// 400 bad request							
							
							std::string body_str( "<html><body><h2>Could not parse request</h2>\r\n</body></html>\r\n" );							
							std::string header( "HTTP/1.1 400 Bad Request\r\nConnection: close\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string( body_str.size( ) ) + "\r\n\r\n" );
							socket->write_async( header + body_str );
							socket->end( body_str );
						}

					}	// namespace anonymous


					HttpConnectionImpl::HttpConnectionImpl( lib::net::NetSocketStream&& socket, base::EventEmitter emitter ) :
						m_socket( std::move( socket ) ),
						m_emitter( std::move( emitter) ) {
					}

					void HttpConnectionImpl::start( ) {
						auto obj = get_weak_ptr( );
						m_socket->on_next_data_received( [obj] ( std::shared_ptr<base::data_t> data_buffer, bool ) mutable {
							if( data_buffer ) {
								run_if_valid( obj, "Exception in processing received data", "HttpConnectionImpl::start#on_next_data_received", [&]( HttpConnection self ) {
									try {
										auto request = parse_http_request( data_buffer->begin( ), data_buffer->end( ) );
										data_buffer.reset( );
										if( request ) {
											auto response = create_http_server_response( self->m_socket->get_weak_ptr( ) );
											response->start( );
											self->emit_request_made( request, response );
										} else {
											err400( self->m_socket );
										}
									} catch( std::exception const & ) {
										err400( self->m_socket );
									}
								} );
							} else {
								throw std::runtime_error( "Null buffer passed to NetSocketStream->on_data_received event" );
							}
						} ).delegate_to( "closed", obj, "closed" )
							.on_error( obj, "HttpConnectionImpl::start" )
							.set_read_mode( lib::net::impl::NetSocketStreamImpl::ReadUntil::double_newline );

						m_socket->read_async( );
					}
 
					base::EventEmitter& HttpConnectionImpl::emitter( ) {
						return m_emitter;
					}

					void HttpConnectionImpl::close( ) {
						if( m_socket ) {
							m_socket->close( );
						}
					}

					void HttpConnectionImpl::emit_closed( ) {
						emitter( )->emit( "closed" );
					}

					void HttpConnectionImpl::emit_client_error( base::Error error ) {
						emitter( )->emit( "client_error", error );
					}

					void HttpConnectionImpl::emit_request_made( HttpClientRequest request, HttpServerResponse response ) {
						emitter( )->emit( "request_made", request, response );
					}


					// Event callbacks

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the connection is closed
					HttpConnectionImpl& HttpConnectionImpl::on_closed( std::function<void( )> listener ) {
						emitter( )->add_listener( "closed", listener, true );
						return *this;
					}

					HttpConnectionImpl& HttpConnectionImpl::on_client_error( std::function<void( base::Error )> listener ) {
						emitter( )->add_listener( "client_error", listener );
						return *this;
					}

					HttpConnectionImpl& HttpConnectionImpl::on_next_client_error( std::function<void( base::Error )> listener ) {
						emitter( )->add_listener( "client_error", listener, true );
						return *this;
					}

					HttpConnectionImpl& HttpConnectionImpl::on_request_made( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
						emitter( )->add_listener( "request_made", listener );
						return *this;
					}

					HttpConnectionImpl& HttpConnectionImpl::on_next_request_made( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
						emitter( )->add_listener( "request_made", listener, true );
						return *this;
					}

					lib::net::NetSocketStream HttpConnectionImpl::socket( ) {
						return m_socket;
					}

				}	// namespace impl

				HttpConnection create_http_connection( lib::net::NetSocketStream&& socket, base::EventEmitter emitter ) {
					return HttpConnection( new impl::HttpConnectionImpl( std::move( socket ), std::move( emitter ) ) );
				}

			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
