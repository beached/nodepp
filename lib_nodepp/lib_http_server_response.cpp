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

#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cstdint>
#include <string>

#include "base_enoding.h"
#include "base_stream.h"
#include "lib_http.h"
#include "lib_http_headers.h"
#include "lib_http_server_response.h"
#include <daw/daw_range_algorithm.h>
#include <daw/daw_string.h>

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				namespace impl {
					HttpServerResponseImpl::HttpServerResponseImpl( std::weak_ptr<lib::net::impl::NetSocketStreamImpl> socket, base::EventEmitter emitter ):
						daw::nodepp::base::StandardEvents<HttpServerResponseImpl>( std::move( emitter ) ),
						m_socket( socket ),
						m_version( 1, 1 ),
						m_headers( ),
						m_body( ),
						m_status_sent( false ),
						m_headers_sent( false ),
						m_body_sent( false ) { }

					void HttpServerResponseImpl::start( ) {
						auto obj = this->get_weak_ptr( );
						on_socket_if_valid( [obj]( lib::net::NetSocketStream socket ) {
							socket->on_write_completion( [obj]( auto ) {
								if( !obj.expired( ) ) {
									obj.lock( )->emit_write_completion( );
								}
							} ).on_all_writes_completed( [obj]( auto ) {
								if( !obj.expired( ) ) {
									obj.lock( )->emit_all_writes_completed( );
								}
							} );
						} );
					}

					bool HttpServerResponseImpl::on_socket_if_valid( std::function<void( lib::net::NetSocketStream )> action ) {
						if( m_socket.expired( ) ) {
							return false;
						}
						action( m_socket.lock( ) );
						return true;
					}

					HttpServerResponseImpl::~HttpServerResponseImpl( ) {}

					HttpServerResponseImpl& HttpServerResponseImpl::write( base::data_t const & data ) {
						m_body.insert( std::end( m_body ), std::begin( data ), std::end( data ) );
						return *this;
					}

					HttpServerResponseImpl& HttpServerResponseImpl::write( boost::string_ref data, base::Encoding const & ) {
						m_body.insert( std::end( m_body ), std::begin( data ), std::end( data ) );
						return *this;
					}

					HttpServerResponseImpl& HttpServerResponseImpl::clear_body( ) {
						m_body.clear( );
						return *this;
					}

					HttpHeaders& HttpServerResponseImpl::headers( ) {
						return m_headers;
					}

					HttpHeaders const & HttpServerResponseImpl::headers( ) const {
						return m_headers;
					}

					daw::nodepp::base::data_t const & HttpServerResponseImpl::body( ) const {
						return m_body;
					}

					HttpServerResponseImpl& HttpServerResponseImpl::send_status( uint16_t status_code ) {
						auto status = HttpStatusCodes( status_code );
						std::string msg = "HTTP/" + m_version.to_string( ) + " " + std::to_string( status.first ) + " " + status.second + "\r\n";

						m_status_sent = on_socket_if_valid( [&msg]( lib::net::NetSocketStream socket ) {
							socket->write_async( msg ); // TODO: make faster
						} );
						return *this;
					}

					HttpServerResponseImpl& HttpServerResponseImpl::send_status( uint16_t status_code, boost::string_ref status_msg ) {
						std::string msg = "HTTP/" + m_version.to_string( ) + " " + std::to_string( status_code ) + " " + status_msg.to_string( ) + "\r\n";

						m_status_sent = on_socket_if_valid( [&msg]( lib::net::NetSocketStream socket ) {
							socket->write_async( msg ); // TODO: make faster
						} );
						return *this;
					}

					namespace {
						std::string gmt_timestamp( ) {
							auto now = time( 0 );
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif
							auto ptm = gmtime( &now );

							char buf[80];
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif
							strftime( buf, sizeof( buf ), "%a, %d %b %Y %H:%M:%S GMT", ptm );

							return buf;
						}
					}

					HttpServerResponseImpl& HttpServerResponseImpl::send_headers( ) {
						m_headers_sent = on_socket_if_valid( [&]( lib::net::NetSocketStream socket ) {
							auto& dte = m_headers["Date"];
							if( dte.empty( ) ) {
								dte = gmt_timestamp( );
							}
							socket->write_async( m_headers.to_string( ) );
						} );
						return *this;
					}

					HttpServerResponseImpl& HttpServerResponseImpl::send_body( ) {
						m_body_sent = on_socket_if_valid( [&]( lib::net::NetSocketStream socket ) {
							HttpHeader content_header( "Content-Length", std::to_string( m_body.size( ) ) );
							socket->write_async( content_header.to_string( ) );
							socket->write_async( "\r\n\r\n" );
							socket->write_async( m_body );
						} );
						return *this;
					}

					bool HttpServerResponseImpl::send( ) {
						bool result = false;
						if( !m_status_sent ) {
							result = true;
							send_status( );
						}
						if( !m_headers_sent ) {
							result = true;
							send_headers( );
						}
						if( !m_body_sent ) {
							result = true;
							send_body( );
						}
						return result;
					}

					HttpServerResponseImpl& HttpServerResponseImpl::end( ) {
						send( );
						on_socket_if_valid( []( lib::net::NetSocketStream socket ) { socket->end( ); } );
						return *this;
					}

					HttpServerResponseImpl& HttpServerResponseImpl::end( base::data_t const & data ) {
						write( data );
						end( );
						return *this;
					}

					HttpServerResponseImpl& HttpServerResponseImpl::end( boost::string_ref data, base::Encoding const & encoding ) {
						write( data, encoding );
						end( );
						return *this;
					}

					void HttpServerResponseImpl::close( ) {
						send( );
						on_socket_if_valid( []( lib::net::NetSocketStream socket ) {
							socket->close( );
						} );
					}

					HttpServerResponseImpl& HttpServerResponseImpl::reset( ) {
						m_status_sent = false;
						m_headers.headers.clear( );
						m_headers_sent = false;
						clear_body( );
						m_body_sent = false;
						return *this;
					}

					bool HttpServerResponseImpl::is_closed( ) const {
						return m_socket.expired( ) || m_socket.lock( )->is_closed( );
					}

					bool HttpServerResponseImpl::can_write( ) const {
						return !m_socket.expired( ) && m_socket.lock( )->can_write( );
					}

					bool HttpServerResponseImpl::is_open( ) {
						return !m_socket.expired( ) && m_socket.lock( )->is_open( );
					}

					HttpServerResponseImpl& HttpServerResponseImpl::add_header( std::string header_name, std::string header_value ) {
						m_headers.add( std::move( header_name ), std::move( header_value ) );
						return *this;
					}
				}	// namespace impl

				HttpServerResponse create_http_server_response( std::weak_ptr<lib::net::impl::NetSocketStreamImpl> socket, base::EventEmitter emitter ) {
					return HttpServerResponse( new impl::HttpServerResponseImpl( std::move( socket ), std::move( emitter ) ) );
				}
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw

