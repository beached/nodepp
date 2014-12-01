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

				std::vector<std::string> const & HttpServerResponse::valid_events( ) const {
					static auto const result = [&]( ) {
						static std::vector<std::string> const local{ "request", "connection", "close", "checkContinue", "connect", "upgrade", "clientError", "listening" };
						auto parent = base::stream::StreamWritable::valid_events( );
						return base::impl::append_vector( local, parent );
					}();
					return result;
				}

				HttpServerResponse::HttpServerResponse( ) : base::stream::StreamWritable( ) { }

				HttpServerResponse& HttpServerResponse::write_continue( ) { throw std::runtime_error( "Method not implemented" ); }
				HttpServerResponse& HttpServerResponse::write_head( uint16_t status_code, std::string reason_phrase, Headers headers ) { throw std::runtime_error( "Method not implemented" ); }


				uint16_t& HttpServerResponse::status_code( ) { throw std::runtime_error( "Method not implemented" ); }
				uint16_t const & HttpServerResponse::status_code( ) const { throw std::runtime_error( "Method not implemented" ); }

				void HttpServerResponse::set_header( std::string name, std::string value ) { throw std::runtime_error( "Method not implemented" ); }
				void HttpServerResponse::set_header( Headers headers ) { throw std::runtime_error( "Method not implemented" ); }

				bool HttpServerResponse::headers_sent( ) const { throw std::runtime_error( "Method not implemented" ); }

				bool& HttpServerResponse::send_date( ) { throw std::runtime_error( "Method not implemented" ); }
				bool const& HttpServerResponse::send_date( ) const { throw std::runtime_error( "Method not implemented" ); }

				HttpHeader const & HttpServerResponse::get_header( std::string name ) const { throw std::runtime_error( "Method not implemented" ); }
				HttpServerResponse& HttpServerResponse::remove_header( std::string name ) { throw std::runtime_error( "Method not implemented" ); }

				bool HttpServerResponse::write_chunk( std::string const & chunk, base::Encoding const & encoding ) { throw std::runtime_error( "Method not implemented" ); }
				bool HttpServerResponse::write_chunk( base::data_t const & chunk, base::Encoding const & encoding ) { throw std::runtime_error( "Method not implemented" ); }
				bool HttpServerResponse::add_trailers( Headers headers ) { throw std::runtime_error( "Method not implemented" ); }


				HttpServerResponse& HttpServerResponse::write( base::data_t const & chunk ) { throw std::runtime_error( "Method not implemented" ); }
				HttpServerResponse& HttpServerResponse::write( std::string const & chunk, base::Encoding const & encoding ) { throw std::runtime_error( "Method not implemented" ); }

				HttpServerResponse& HttpServerResponse::end( ) { throw std::runtime_error( "Method not implemented" ); }
				HttpServerResponse& HttpServerResponse::end( base::data_t const & chunk ) { throw std::runtime_error( "Method not implemented" ); }
				HttpServerResponse& HttpServerResponse::end( std::string const & chunk, base::Encoding const & encoding ) { throw std::runtime_error( "Method not implemented" ); }
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
