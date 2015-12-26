// The MIT License (MIT)
//
// Copyright (c) 2014-2015 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "lib_http_parser_impl.h"
#include <boost/utility/string_ref.h>
#include <boost/spirit/home/qi/parse.hpp>

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				std::shared_ptr<daw::nodepp::lib::http::impl::HttpClientRequestImpl> parse_http_request( daw::nodepp::base::data_t::iterator first, daw::nodepp::base::data_t::iterator last ) {
					auto result = std::make_shared < daw::nodepp::lib::http::impl::HttpClientRequestImpl >( );
					if( !boost::spirit::qi::parse( first, last, daw::nodepp::lib::http::request_parser::http_request_parse_grammar<decltype(first)>( ), *result ) ) {
						result = nullptr;
					}
					return result;
				}

				std::shared_ptr<daw::nodepp::lib::http::impl::HttpUrlImpl> parse_url( boost::string_ref url_string ) {
					auto result = std::make_shared<daw::nodepp::lib::http::impl::HttpUrlImpl>( );
					if( !boost::spirit::qi::parse( url_string.begin( ), url_string.end( ), daw::nodepp::lib::http::request_parser::url_parse_grammar<decltype(url_string.begin( ))>( ), *result ) ) {
						result = nullptr;
					}
					return result;
				}

			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw

