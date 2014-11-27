#include <cstdint>
#include <string>

#include "base_enoding.h"
#include "base_event_emitter.h"
#include "lib_http.h"
#include "lib_http_headers.h"
#include "lib_http_server_response.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp::base;

				std::vector<std::string> const & HttpServerResponse::valid_events( ) const {
					static auto const result = [&]( ) {
						std::vector<std::string> local{ "request", "connection", "close", "checkContinue", "connect", "upgrade", "clientError", "listening" };
						auto parent = EventEmitter::valid_events( );
						return impl::append_vector( local, parent );
					}();
					return result;
				}

				HttpServerResponse::HttpServerResponse( ) : daw::nodepp::base::EventEmitter{ } { }

				HttpServerResponse& HttpServerResponse::write_continue( ) { throw std::runtime_error( "Method Not Implemented" ); }
				HttpServerResponse& HttpServerResponse::write_head( uint16_t status_code, std::string reason_phrase, Headers headers ) { throw std::runtime_error( "Method Not Implemented" ); }


				uint16_t& HttpServerResponse::status_code( ) { throw std::runtime_error( "Method Not Implemented" ); }
				uint16_t const & HttpServerResponse::status_code( ) const { throw std::runtime_error( "Method Not Implemented" ); }

				void HttpServerResponse::set_header( std::string name, std::string value ) { throw std::runtime_error( "Method Not Implemented" ); }
				void HttpServerResponse::set_header( Headers headers ) { throw std::runtime_error( "Method Not Implemented" ); }

				bool HttpServerResponse::headers_sent( ) const { throw std::runtime_error( "Method Not Implemented" ); }

				bool& HttpServerResponse::send_date( ) { throw std::runtime_error( "Method Not Implemented" ); }
				bool const& HttpServerResponse::send_date( ) const { throw std::runtime_error( "Method Not Implemented" ); }

				HttpHeader const & HttpServerResponse::get_header( std::string name ) const { throw std::runtime_error( "Method Not Implemented" ); }
				HttpServerResponse& HttpServerResponse::remove_header( std::string name ) { throw std::runtime_error( "Method Not Implemented" ); }

				bool HttpServerResponse::write_chunk( std::string const & chunk, base::Encoding encoding ) { throw std::runtime_error( "Method Not Implemented" ); }
				bool HttpServerResponse::write_chunk( HttpChunk const & chunk ) { throw std::runtime_error( "Method Not Implemented" ); }
				bool HttpServerResponse::add_trailers( Headers headers ) { throw std::runtime_error( "Method Not Implemented" ); }

				void HttpServerResponse::end( ) { throw std::runtime_error( "Method Not Implemented" ); }
				void HttpServerResponse::end( std::string const & chunk, base::Encoding encoding ) { throw std::runtime_error( "Method Not Implemented" ); }
				void HttpServerResponse::end( HttpChunk chunk ) { throw std::runtime_error( "Method Not Implemented" ); }


			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
