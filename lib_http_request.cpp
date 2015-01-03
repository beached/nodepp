
#include <boost/spirit/include/qi_parse_attr.hpp>
#include <ostream>

#include "base_types.h"
#include "lib_http_request.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {

				std::string to_string( HttpClientRequestMethod method ) {
					switch( method ) {
					case HttpClientRequestMethod::Get:
						return "Get";
					case HttpClientRequestMethod::Post:
						return "Post";
					case HttpClientRequestMethod::Connect:
						return "Connect";
					case HttpClientRequestMethod::Delete:
						return "Delete";
					case HttpClientRequestMethod::Head:
						return "Head";
					case HttpClientRequestMethod::Options:
						return "Options";
					case HttpClientRequestMethod::Put:
						return "Put";
					case HttpClientRequestMethod::Trace:
						return "Trace";
					case HttpClientRequestMethod::Any:
						return "Any";
					}

					throw std::runtime_error( "Unrecognized HttpRequestMethod" );
				}

				std::string to_string( HttpUrl const & url ) {
					return "{ " + url.path + ", " + url.query.value_or( "" ) + " } ";
				}

				std::string to_string( HttpRequestLine const & request_line ) {
					std::stringstream ss;
					ss << "{ " << to_string( request_line.method ) << ", " << to_string( request_line.url ) << ", " << request_line.version << " } ";
					return ss.str( );
				}

				std::ostream& operator<<(std::ostream& os, HttpClientRequestMethod const method) {
					os << to_string( method );
					return os;
				}

				std::ostream& operator<<(std::ostream& os, HttpUrl const & url) {
					os << to_string( url );
					return os;
				}

				std::ostream& operator<<(std::ostream& os, HttpRequestLine const & request) {
					//os << "{ " << to_string( request.method ) << ", " << request.url << ", " << request.version << " } ";
					os << to_string( request );
					return os;
				}

				namespace impl {
					std::ostream& operator<<(std::ostream& os, HttpClientRequestImpl const & req) {
						os << req.request;
						return os;
					}
				}	// namespace impl

			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw


