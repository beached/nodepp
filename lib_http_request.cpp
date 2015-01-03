
#include <boost/spirit/include/qi_parse_attr.hpp>
#include <ostream>

#include "base_types.h"
#include "lib_http_request.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {

				std::string http_request_method_as_string( HttpClientRequestMethod method ) {
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

				namespace {
					std::string to_string( HttpRequestLine const & request ) {
						std::stringstream ss;
						ss << "{ " << http_request_method_as_string( request.method ) << ", " << request.url.path << ", " << request.url.query << ", " << request.version << " } ";
						return ss.str( );
					}
				}

				std::ostream& operator<<(std::ostream& os, HttpClientRequestMethod const method) {
					os << http_request_method_as_string( method );
					return os;
				}

				std::ostream& operator<<(std::ostream& os, HttpUrl const & url) {
					os << "{ " << url.path << ", " << url.query << " } ";
					return os;
				}

				std::ostream& operator<<(std::ostream& os, HttpRequestLine const & request) {
					os << to_string( request );
					return os;
				}

				namespace impl {
					std::ostream& operator<<(std::ostream& os, HttpClientRequestImpl const & req) {
						os << lib::http::to_string( req.request );
						return os;
					}
				}

			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw


