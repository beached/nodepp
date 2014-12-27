
#include <boost/spirit/include/qi_parse_attr.hpp>
#include "base_types.h"
#include "lib_http_request.h"
#include "lib_http_request_parser_impl.h"

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

				std::shared_ptr<daw::nodepp::lib::http::impl::HttpClientRequestImpl> parse_http_request( daw::nodepp::base::data_t::iterator first, daw::nodepp::base::data_t::iterator last ) {
					auto result = std::make_shared < daw::nodepp::lib::http::impl::HttpClientRequestImpl >( );
					if( !boost::spirit::qi::parse( first, last, daw::nodepp::lib::http::request_parser::parse_grammar<decltype(first)>( ), *result ) ) {
						result = nullptr;
					}
					return result;
				}

			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
