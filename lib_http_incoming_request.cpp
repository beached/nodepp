#include <sstream>
#include <boost/lexical_cast.hpp>

#include "lib_http_incoming_request.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;	

				HttpHeaders const & HttpIncomingRequest::headers( ) const { throw std::runtime_error( "Method not implemented" ); }
				HttpHeaders & HttpIncomingRequest::headers( ) { throw std::runtime_error( "Method not implemented" ); }

				HttpRequestMethod const & HttpIncomingRequest::method( ) const { throw std::runtime_error( "Method not implemented" ); }
				HttpRequestMethod & HttpIncomingRequest::method( ) { throw std::runtime_error( "Method not implemented" ); }

				HttpHeaders const & HttpIncomingRequest::trailers( ) const { throw std::runtime_error( "Method not implemented" ); }
				HttpHeaders & HttpIncomingRequest::trailers( ) { throw std::runtime_error( "Method not implemented" ); }

				HttpVersion const & HttpIncomingRequest::version( ) const { throw std::runtime_error( "Method not implemented" ); }
				HttpVersion & HttpIncomingRequest::version( ) { throw std::runtime_error( "Method not implemented" ); }

				base::Url const & HttpIncomingRequest::url( ) const { throw std::runtime_error( "Method not implemented" ); }
				base::Url & HttpIncomingRequest::url( ) { throw std::runtime_error( "Method not implemented" ); }

				uint16_t& HttpIncomingRequest::status( ) { throw std::runtime_error( "Method not implemented" ); }
				uint16_t const & HttpIncomingRequest::status( ) const { throw std::runtime_error( "Method not implemented" ); }

				std::shared_ptr<lib::net::NetSocket> & HttpIncomingRequest::socket( ) { throw std::runtime_error( "Method not implemented" ); }
				std::shared_ptr<lib::net::NetSocket> const & HttpIncomingRequest::socket( ) const { throw std::runtime_error( "Method not implemented" ); }

				// StreamReadable interface
				std::vector<std::string> const & HttpIncomingRequest::valid_events( ) const {
					static auto const result = [&]( ) {
						std::vector<std::string> local{ "close" };
						auto parent = base::stream::StreamReadable::valid_events( );
						return base::impl::append_vector( local, parent );
					}();
					return result;
				}

				 base::data_t HttpIncomingRequest::read( ) { throw std::runtime_error( "Method not implemented" ); }
				 base::data_t HttpIncomingRequest::read( size_t bytes ) { throw std::runtime_error( "Method not implemented" ); }

				 HttpIncomingRequest& HttpIncomingRequest::set_encoding( base::Encoding const & encoding ) { throw std::runtime_error( "Method not implemented" ); }
				 HttpIncomingRequest& HttpIncomingRequest::resume( ) { throw std::runtime_error( "Method not implemented" ); }
				 HttpIncomingRequest& HttpIncomingRequest::pause( ) { throw std::runtime_error( "Method not implemented" ); }
				 base::stream::StreamWritable& HttpIncomingRequest::pipe( base::stream::StreamWritable& destination ) { throw std::runtime_error( "Method not implemented" ); }
				 base::stream::StreamWritable& HttpIncomingRequest::pipe( base::stream::StreamWritable& destination, base::options_t options ) { throw std::runtime_error( "Method not implemented" ); }
				 HttpIncomingRequest& HttpIncomingRequest::unpipe( base::stream::StreamWritable& destination ) { throw std::runtime_error( "Method not implemented" ); }
				 HttpIncomingRequest& HttpIncomingRequest::unshift( base::data_t const & chunk ) { throw std::runtime_error( "Method not implemented" ); }

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
