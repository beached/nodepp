
#include <boost/spirit/include/qi_parse_attr.hpp>
#include <ostream>

#include "base_types.h"
#include "base_json.h"
#include "lib_http_request.h"

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

				std::string json_value( std::string const & name, HttpClientRequestMethod method ) {					
					return base::json::json_value( name, to_string( method ) );
				}

				std::string HttpUrl::serialize_to_json( ) const {
					using namespace base::json;
					std::string result = json_value( "path", path ) + ",\n";
					if( query ) {
						result += json_value( "query", query.get( ) );
					} else {
						result += json_value( "query" );
					}
					return details::enbracket( result );
				}

				std::string HttpRequestLine::serialize_to_json( ) const {
					using namespace base::json;
					std::string result = json_value( "method", method ) + ",\n";					
					result += json_value( "url", url ) + ",\n";
					result += json_value( "version", version );
					return details::enbracket( result );
				}

				namespace impl {
					std::string HttpClientRequestImpl::serialize_to_json( ) const {
						using namespace base::json;
						return details::enbracket( json_value( "request", request ) );
					}


				}	// namespace impl

			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw


