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
				
				
				bool ServerResponse::event_is_valid( std::string const & event ) const {
					static std::vector<std::string> const valid_events = { "close", "finish" };
					return daw::algorithm::find( valid_events, event ) != valid_events.end( ) || EventEmitter::event_is_valid( event );
				}

				ServerResponse::ServerResponse( ) : daw::nodepp::base::EventEmitter{ } { }

				ServerResponse& ServerResponse::write_continue( ) { throw std::runtime_error( "Method Not Implemented" ); }
				ServerResponse& ServerResponse::write_head( uint16_t status_code, std::string reason_phrase, Headers headers ) { throw std::runtime_error( "Method Not Implemented" ); }


				uint16_t& ServerResponse::status_code( ) { throw std::runtime_error( "Method Not Implemented" ); }
				uint16_t const & ServerResponse::status_code( ) const { throw std::runtime_error( "Method Not Implemented" ); }

				void ServerResponse::set_header( std::string name, std::string value ) { throw std::runtime_error( "Method Not Implemented" ); }
				void ServerResponse::set_header( Headers headers ) { throw std::runtime_error( "Method Not Implemented" ); }

				bool ServerResponse::headers_sent( ) const { throw std::runtime_error( "Method Not Implemented" ); }

				bool& ServerResponse::send_date( ) { throw std::runtime_error( "Method Not Implemented" ); }
				bool const& ServerResponse::send_date( ) const { throw std::runtime_error( "Method Not Implemented" ); }

				Header const & ServerResponse::get_header( std::string name ) const { throw std::runtime_error( "Method Not Implemented" ); }
				ServerResponse& ServerResponse::remove_header( std::string name ) { throw std::runtime_error( "Method Not Implemented" ); }

				bool ServerResponse::write_chunk( Chunk chunk, daw::nodepp::base::Encoding encoding ) { throw std::runtime_error( "Method Not Implemented" ); }
				bool ServerResponse::add_trailers( Headers headers ) { throw std::runtime_error( "Method Not Implemented" ); }

				void ServerResponse::end( ) { throw std::runtime_error( "Method Not Implemented" ); }
				void ServerResponse::end( Chunk chunk, daw::nodepp::base::Encoding encoding ) { throw std::runtime_error( "Method Not Implemented" ); }

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
