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

				HttpServerResponse::HttpServerResponse( std::shared_ptr<lib::net::NetSocketStream> socket_ptr ) : 
						m_version( 1, 1 ), 
						m_headers( ), 
						m_body( ), 
						m_status_sent( false ), 
						m_headers_sent( false ), 
						m_body_sent( false ), 
						m_socket_ptr( socket_ptr ) { }

				HttpServerResponse::HttpServerResponse( HttpServerResponse&& other ): 
					m_version( std::move( other.m_version ) ),
					m_headers( std::move( other.m_headers ) ),
					m_body( std::move( other.m_body ) ),
					m_status_sent( std::move( other.m_status_sent) ),
					m_headers_sent( std::move( other.m_headers_sent) ),
					m_body_sent( std::move( other.m_body_sent ) ),
					m_socket_ptr( other.m_socket_ptr ) { }


				HttpServerResponse& HttpServerResponse::operator = (HttpServerResponse && rhs) {
					if( this != &rhs ) {
						m_version = std::move( rhs.m_version );
						m_headers = std::move( rhs.m_headers );
						m_body = std::move( rhs.m_body );
						m_status_sent = std::move( rhs.m_status_sent );
						m_headers_sent = std::move( rhs.m_headers_sent );
						m_body_sent = std::move( rhs.m_body_sent );
						m_socket_ptr = std::move( rhs.m_socket_ptr );
					}
					return *this;
				}

				HttpServerResponse& HttpServerResponse::write( base::data_t data ) {
					m_body.insert( std::end( m_body ), std::begin( data ), std::end( data ) );
					return *this;
				}

				HttpServerResponse& HttpServerResponse::write( std::string data, base::Encoding encoding ) { 
					m_body.insert( std::end( m_body ), std::begin( data ), std::end( data ) );
					return *this;
				}

				void HttpServerResponse::clear_body( ) {
					m_body.clear( );
				}

				HttpHeaders& HttpServerResponse::headers( ) {
					return m_headers;
				}

				HttpHeaders const & HttpServerResponse::headers( ) const {
					return m_headers;
				}

				void HttpServerResponse::send_status( uint16_t status_code ) {
					auto status = HttpStatusCodes( status_code );
					auto msg = daw::string::string_format( "HTTP/{0} {1} {2}\r\n", m_version.to_string( ), status.first, status.second );
					m_socket_ptr->write( msg ); // TODO make faster
					m_status_sent = true;
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

				void HttpServerResponse::send_headers( ) {
					auto& dte = m_headers["Date"];
					if( dte.empty( ) ) {
						dte = gmt_timestamp( );
					}
					m_socket_ptr->write( m_headers.to_string() );
					m_headers_sent = true;
				}
				void HttpServerResponse::send_body( ) {
					HttpHeader content_header( "Content-Length", boost::lexical_cast<std::string>(m_body.size( )) );
					m_socket_ptr->write( content_header.to_string( ) );
					m_socket_ptr->write( "\r\n\r\n" );
					m_socket_ptr->write( m_body );
					m_body_sent = true;
				}				

				void HttpServerResponse::send( ) {
					if( !m_status_sent ) {
						send_status( );
					}
					if( !m_headers_sent ) {
						send_headers( );
					}
					if( !m_body_sent ) {
						send_body( );
					}
				}

				void HttpServerResponse::end( ) {
					send( );
					m_socket_ptr->end( );					
				}

				void HttpServerResponse::end( base::data_t data ) {
					write( std::move( data ) );
					end( );
				}

				void HttpServerResponse::end( std::string data, base::Encoding encoding ) {
					write( std::move( data ), std::move( encoding ) );
					end( );
				}

				void HttpServerResponse::close( ) {
					m_socket_ptr->close( );
				}

				void HttpServerResponse::reset( ) {
					m_status_sent = false;
					m_headers.headers.clear( );
					m_headers_sent = false;
					clear_body( );
					m_body_sent = false;
				}

				bool HttpServerResponse::is_closed( ) const {
					return m_socket_ptr->is_closed( );
				}

				bool HttpServerResponse::can_write( ) const {
					return m_socket_ptr->can_write( );
				}

				bool HttpServerResponse::is_open( ) {
					return m_socket_ptr->is_open( );
				}

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
