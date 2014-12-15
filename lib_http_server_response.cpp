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

				HttpServerResponse::HttpServerResponse( std::shared_ptr<lib::net::NetSocket> socket_ptr ) : m_version( 1, 1 ), m_body( ), m_headers( ), m_status_sent( false ), m_headers_sent( false ), m_body_sent( false ), m_socket_ptr( std::move( socket ) ) { }

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
					auto msg = daw::string::string_format( "{0} {1} {2}", m_version.to_string( ), status.first, status.second );
					m_socket_ptr->write( msg ); // TODO make faster
					m_status_sent = true;
				}

				void HttpServerResponse::send_headers( ) {
					auto dte = m_headers["Date"];
					if( dte.empty( ) ) {

					}
					m_socket_ptr->write( m_headers.to_string() );
					m_headers_sent = true;
				}
				void HttpServerResponse::send_body( ) {
					HttpHeader content_header( "Content-Length", boost::lexical_cast<std::string>(m_body.size( )) );
					m_socket_ptr->write( content_header.to_string( ) );
					m_socket_ptr->write( "\r\n" );
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

				void HttpServerResponse::reset( ) {
					m_status_sent = false;
					m_headers.headers.clear( );
					m_headers_sent = false;
					clear_body( );
					m_body_sent = false;
				}

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
