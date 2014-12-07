
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
				using namespace daw::nodepp;
				HttpClientRequest::HttpClientRequest( ) : base::stream::Stream( ) { }



				std::vector<std::string> const & HttpClientRequest::valid_events( ) const {
					static auto const result = [&]( ) {
						std::vector<std::string> local{ "request", "connection", "close", "checkContinue", "connect", "upgrade", "clientError", "listening" };
						auto parent = EventEmitter::valid_events( );
						return base::impl::append_vector( local, parent );
					}();
					return result;
				}

				base::data_t HttpClientRequest::read( ) { throw std::runtime_error( "Method not implemented" ); }
				base::data_t HttpClientRequest::read( size_t bytes ) { throw std::runtime_error( "Method not implemented" ); }

				HttpClientRequest& HttpClientRequest::set_encoding( base::Encoding const & encoding ) { throw std::runtime_error( "Method not implemented" ); }
				HttpClientRequest& HttpClientRequest::resume( ) { throw std::runtime_error( "Method not implemented" ); }
				HttpClientRequest& HttpClientRequest::pause( ) { throw std::runtime_error( "Method not implemented" ); }
				base::stream::StreamWritable& HttpClientRequest::pipe( base::stream::StreamWritable& destination ) { throw std::runtime_error( "Method not implemented" ); }
				base::stream::StreamWritable& HttpClientRequest::pipe( base::stream::StreamWritable& destination, base::options_t options ) { throw std::runtime_error( "Method not implemented" ); }
				HttpClientRequest& HttpClientRequest::unpipe( StreamWritable& destination ) { throw std::runtime_error( "Method not implemented" ); }
				HttpClientRequest& HttpClientRequest::unshift( base::data_t const & chunk ) { throw std::runtime_error( "Method not implemented" ); }

				// StreamWritable Interface
				HttpClientRequest& HttpClientRequest::write( base::data_t const & chunk ) { throw std::runtime_error( "Method not implemented" ); }
				HttpClientRequest& HttpClientRequest::write( std::string const & chunk, base::Encoding const & encoding ) { throw std::runtime_error( "Method not implemented" ); }

				HttpClientRequest& HttpClientRequest::end( ) { throw std::runtime_error( "Method not implemented" ); }
				HttpClientRequest& HttpClientRequest::end( base::data_t const & chunk ) { throw std::runtime_error( "Method not implemented" ); }
				HttpClientRequest& HttpClientRequest::end( std::string const & chunk, base::Encoding const & encoding ) { throw std::runtime_error( "Method not implemented" ); }

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
