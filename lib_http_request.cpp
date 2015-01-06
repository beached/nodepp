
#include <boost/spirit/include/qi_parse_attr.hpp>
#include <ostream>

#include "base_types.h"
#include "lib_http_request.h"
#include "base_json.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				using namespace daw::nodepp::base::json;

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

				std::string value_to_json( std::string const & name, HttpClientRequestMethod method ) {
					return base::json::value_to_json( name, to_string( method ) );
				}

				std::string value_to_json( std::string const & name, HttpUrlQueryPair const & query_pair ) {
					std::string result = base::json::value_to_json( "name", query_pair.name ) + ",\n";
					result += base::json::value_to_json( "value", query_pair.value );
					return details::json_name( name ) + details::enbracket( result );
				}

				std::string value_to_json( std::string const & name, HttpUrl const & url ) {
					std::string result = base::json::value_to_json( "path", url.path ) + ",\n";
					result += base::json::value_to_json( "query", url.query );
					result += base::json::value_to_json( "fragment", url.fragment );
					return details::json_name( name ) + details::enbracket( result );
				}

				std::ostream& operator<<(std::ostream& os, HttpClientRequestMethod method) {
					os << value_to_json( "HttpClientRequestMethod", method );
					return os;
				}

				std::string value_to_json( std::string const & name, HttpRequestLine const & request_line ) {
					std::string result = value_to_json( "method", request_line.method ) + ",\n";
					result += value_to_json( "url", request_line.url ) + ",\n";
					result += base::json::value_to_json( "version", request_line.version );
					return details::json_name( name ) + details::enbracket( result );
				}

				namespace impl {
					std::string value_to_json( std::string const & name, HttpClientRequestImpl const & client_request ) {
						return details::json_name( name ) + details::enbracket( value_to_json( "request", client_request.request ) );
					}


				}	// namespace impl

			} // namespace http
		}	// namespace lib
	
	}	// namespace nodepp
}	// namespace daw


