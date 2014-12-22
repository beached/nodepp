#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cstdint>
#include <string>

#include "base_enoding.h"
#include "base_stream.h"
#include "lib_http.h"
#include "lib_http_headers.h"
#include "lib_http_server_response.h"
#include "range_algorithm.h"
#include "string.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				namespace impl {

					HttpServerResponseImpl::HttpServerResponseImpl( lib::net::NetSocketStream socket, base::EventEmitter emitter ) :
						m_emitter( emitter ),
						m_socket( std::move( socket ) ),
						m_version( 1, 1 ),
						m_headers( ),
						m_body( ),
						m_status_sent( false ),
						m_headers_sent( false ),
						m_body_sent( false ) { }

					void HttpServerResponseImpl::start( ) {
						std::weak_ptr<HttpServerResponseImpl> obj( get_ptr( ) );
						m_socket->on_write_completion( [obj]( ) {
							if( !obj.expired( ) ) {
								obj.lock( )->emit_write_completion( );
							}
						} ).on_all_writes_completed( [obj]( ) {
							if( !obj.expired( ) ) {
								obj.lock( )->emit_all_writes_completed( );
							}
						} );
					}

					std::shared_ptr<HttpServerResponseImpl> HttpServerResponseImpl::get_ptr( ) {
						return shared_from_this( );
					}

					base::EventEmitter& HttpServerResponseImpl::emitter( ) {
						return m_emitter;
					}

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

					HttpServerResponseImpl& HttpServerResponseImpl::send_status( uint16_t status_code ) {
						auto status = HttpStatusCodes( status_code );
						std::string msg = "HTTP/" + m_version.to_string( ) + " " + std::to_string( status.first ) + " " + status.second + "\r\n";
						//auto msg = daw::string::string_format( "HTTP/{0} {1} {2}\r\n", m_version.to_string( ), status.first, status.second );
						m_socket->write_async( msg ); // TODO make faster
						m_status_sent = true;
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
						auto& dte = m_headers["Date"];
						if( dte.empty( ) ) {
							dte = gmt_timestamp( );
						}
						m_socket->write_async( m_headers.to_string( ) );
						m_headers_sent = true;
						return *this;
					}

					HttpServerResponseImpl& HttpServerResponseImpl::send_body( ) {
						HttpHeader content_header( "Content-Length", boost::lexical_cast<std::string>(m_body.size( )) );
						m_socket->write_async( content_header.to_string( ) );
						m_socket->write_async( "\r\n\r\n" );
						m_socket->write_async( m_body );
						m_body_sent = true;
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
						m_socket->end( );
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
						if( !send( ) ) {
							m_socket->close( );
						}
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
						return m_socket->is_closed( );
					}

					bool HttpServerResponseImpl::can_write( ) const {
						return m_socket->can_write( );
					}

					bool HttpServerResponseImpl::is_open( ) {
						return m_socket->is_open( );
					}

					HttpServerResponseImpl& HttpServerResponseImpl::add_header( std::string header_name, std::string header_value ) {
						m_headers.add( std::move( header_name ), std::move( header_value ) );
						return *this;
					}

				}	// namespace impl

				HttpServerResponse create_http_server_response( lib::net::NetSocketStream socket, base::EventEmitter emitter ) {
					return HttpServerResponse( new impl::HttpServerResponseImpl( socket, emitter ) );
				}
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
