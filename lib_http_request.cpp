
#include <boost/spirit/include/qi_parse_attr.hpp>
#include "base_types.h"
#include "lib_http_request.h"
#include "lib_http_request_parser_impl.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {

				std::string http_request_method_as_string( HttpRequestMethod method ) {
					switch( method ) {
					case HttpRequestMethod::Get:
						return "Get";
					case HttpRequestMethod::Post:
						return "Post";
					case HttpRequestMethod::Connect:
						return "Connect";
					case HttpRequestMethod::Delete:
						return "Delete";
					case HttpRequestMethod::Head:
						return "Head";
					case HttpRequestMethod::Options:
						return "Options";
					case HttpRequestMethod::Put:
						return "Put";
					case HttpRequestMethod::Trace:
						return "Trace";
					case HttpRequestMethod::Any:
						return "Any";
					}
					throw std::runtime_error( "Unrecognized HttpRequestMethod" );
				}

				std::shared_ptr<daw::nodepp::lib::http::HttpClientRequest> parse_http_request( daw::nodepp::base::data_t::iterator first, daw::nodepp::base::data_t::iterator last ) {
					auto result = std::make_shared < daw::nodepp::lib::http::HttpClientRequest >( );
					if( !boost::spirit::qi::parse( first, last, daw::nodepp::lib::http::request_parser::parse_grammar<decltype(first)>( ), *result ) ) {
						result = nullptr;
					}
					return result;
				}

			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
