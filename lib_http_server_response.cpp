#include <cstdint>
#include <string>

#include "base_enoding.h"
#include "base_stream.h"
#include "lib_http.h"
#include "lib_http_headers.h"
#include "lib_http_server_response.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;

				HttpServerResponse::HttpServerResponse( uint16_t code ) : m_version( 1, 1 ), m_status( HttpStatusCodes( code ) ), m_body( ), m_headers( ) { }

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

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
