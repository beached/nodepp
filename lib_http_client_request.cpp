
#include <string>

#include "base_enoding.h"
#include "base_event_emitter.h"
#include "lib_http_chunk.h"
#include "lib_http_client_request.h"
#include "lib_types.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp::base;
				bool HttpClientRequest::event_is_valid( std::string const & event ) const {
					static std::vector<std::string> const valid_events = { "response", "socket", "connect", "upgrade", "continued", "newListener", "removeListener" };
					return daw::algorithm::find( valid_events, event ) != valid_events.end( ) || EventEmitter::event_is_valid( event );
				}


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
