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

				HttpServerResponse::HttpServerResponse( ) : version( 1, 1 ), status( HttpStatusCodes( 200 ) ), body( ), is_chunk( false ), next( ) { }

				HttpServerResponse& HttpServerResponse::write( base::data_t data ) {
					body.insert( std::end( body ), std::begin( data ), std::end( data ) );
				}

				HttpServerResponse& HttpServerResponse::write( std::string data, base::Encoding encoding = base::Encoding( ) ) { 
					body.insert( std::end( body ), std::begin( data ), std::end( data ) );
				}

				void HttpServerResponse::clear_body( ) {
					body.clear( );
				}

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
