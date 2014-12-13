
#include <boost\spirit\home\qi\parse_attr.hpp>

#include "base_types.h"
#include "lib_http_request.h"
#include "lib_http_request_parser_impl.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {

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