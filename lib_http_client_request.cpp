
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
				bool ClientRequest::event_is_valid( std::string const & event ) const {
					static std::vector<std::string> const valid_events = { "response", "socket", "connect", "upgrade", "continued", "newListener", "removeListener" };
					return daw::algorithm::find( valid_events, event ) != valid_events.end( ) || EventEmitter::event_is_valid( event );
				}


				ClientRequest::ClientRequest( ) : EventEmitter{ } { }
				bool ClientRequest::write( std::string chunk, daw::nodepp::base::Encoding const & encoding ) { throw std::runtime_error( "Method Not Implemented" ); }
				bool ClientRequest::write( Chunk const & chunk ) { throw std::runtime_error( "Method Not Implemented" ); }

				void ClientRequest::end( ) { throw std::runtime_error( "Method Not Implemented" ); }
				void ClientRequest::end( std::string const & data, daw::nodepp::base::Encoding const & encoding ) { throw std::runtime_error( "Method Not Implemented" ); }
				void ClientRequest::end( Chunk const & chunk ) { throw std::runtime_error( "Method Not Implemented" ); }
				void ClientRequest::abort( ) { throw std::runtime_error( "Method Not Implemented" ); }

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
