
#include <string>

#include "base_enoding.h"
#include "base_event_emitter.h"
#include "base_types.h"
#include "lib_http_chunk.h"
#include "lib_http_client_request.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp::base;

				std::vector<std::string> const & HttpClientRequest::valid_events( ) const {
					static auto const result = [&]( ) {
						std::vector<std::string> local{ "request", "connection", "close", "checkContinue", "connect", "upgrade", "clientError", "listening" };
						auto parent = EventEmitter::valid_events( );
						return daw::nodepp::base::impl::append_vector( local, parent );
					}();
					return result;
				}

				// DAW
// 				bool HttpClientRequest::event_is_valid( std::string const & event ) const {
// 					static std::vector<std::string> const valid_events = { "response", "socket", "connect", "upgrade", "continued" };
// 					return daw::algorithm::find( valid_events, event ) != valid_events.end( ) || EventEmitter::event_is_valid( event );
// 				}


				HttpClientRequest::HttpClientRequest( ) : EventEmitter{ } { }
				bool HttpClientRequest::write( std::string chunk, daw::nodepp::base::Encoding const & encoding ) { throw std::runtime_error( "Method Not Implemented" ); }
				bool HttpClientRequest::write( HttpChunk const & chunk ) { throw std::runtime_error( "Method Not Implemented" ); }

				void HttpClientRequest::end( ) { throw std::runtime_error( "Method Not Implemented" ); }
				void HttpClientRequest::end( std::string const & data, daw::nodepp::base::Encoding const & encoding ) { throw std::runtime_error( "Method Not Implemented" ); }
				void HttpClientRequest::end( HttpChunk const & chunk ) { throw std::runtime_error( "Method Not Implemented" ); }
				void HttpClientRequest::abort( ) { throw std::runtime_error( "Method Not Implemented" ); }

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
