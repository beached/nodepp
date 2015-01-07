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

#include "base_json_impl.h"
#include <boost/config/warning_disable.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/optional.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

BOOST_FUSION_ADAPT_STRUCT(
	daw::nodepp::base::json::impl::json_obj_impl,
	(std::map<std::string, boost::optional<boost::string_ref>>, val_members)
	(std::map<std::string, boost::optional<json_obj_impl>>, obj_members)
	)
		

namespace daw {
	namespace nodepp {
		namespace base {
			namespace json {
				namespace impl {
					namespace ascii = boost::spirit::ascii;
					namespace qi = boost::spirit::qi;
					namespace spirit = boost::spirit;

					using ascii::char_;
					using qi::lexeme;
					using qi::lit;
					using qi::raw;
					using qi::long_long;
					using qi::double_;
					using qi::bool_;
					using spirit::omit;

					template<typename Iterator>
					struct json_obj_impl_parser: qi::grammar < Iterator, daw::nodepp::base::json::impl::json_obj_impl( ) > {
						json_obj_impl_parser( ) : json_obj_impl_parser::base_type( obj_members ) {
							hex = _char["0-9a-fA-F"];
							escaped = lit["\\u"] >> qi::repeat( 4 )[hex];
							nulls = lit["null"];
							bools =-bool_;
							numbers = long_long | double_;
							strings = lit['"'] >> *(hex|char_) >> lit['"'];
							objs = lit["{"] >> lit["}"];
						}
						qi::rule<Iterator> hex;
						qi::rule<Iterator, std::vector<char>> escaped;
						qi::rule<Iterator, boost::string_ref> numbers, bools, strings, arrays, nulls, objs;
						qi::rule < Iterator, std::map<std::string, boost::optional<boost::string_ref>> val_members;
						qi::rule < Iterator, std::map<std::string, boost::optional<boost::string_ref>> obj_members;
					};	// struct json_obj_impl_parser

					json_obj parse_json( std::string const & json_text ) {

					}
				}	// namespace impl
			}	// namespace json
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw

