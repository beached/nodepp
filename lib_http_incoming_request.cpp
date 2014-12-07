#include <sstream>
#include <boost/lexical_cast.hpp>

#include "lib_http_incoming_request.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;

				std::vector<std::string> const & HttpIncomingRequest::valid_events( ) const {
					static auto const result = [&]( ) {
						std::vector<std::string> local{ "close" };
						auto parent = base::stream::StreamReadable::valid_events( );
						return base::impl::append_vector( local, parent );
					}();
					return result;
				}

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
