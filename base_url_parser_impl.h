#pragma once

#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/spirit/include/qi.hpp>
#include <map>
#include <string>

#include "base_url.h"
#include "base_types.h"

BOOST_FUSION_ADAPT_STRUCT(
	daw::nodepp::base::Url,
	(boost::optional< std::string >, root)
	(boost::optional< std::string>, hierarchy)
	(boost::optional< query_t >, queries)
)

namespace daw {
	namespace nodepp {
		namespace base {
			namespace url_parser {
				using namespace daw::nodepp;
				namespace qi = boost::spirit::qi;
				namespace ascii = boost::spirit::ascii;
				using qi::lit;
				using ascii::char_;

				template <typename Iterator >
				struct url_encoded_data_grammar: qi::grammar < Iterator, daw::nodepp::base::Url( ) > {
					url_encoded_data_grammar( ) : url_encoded_data_grammar::base_type( start ) {
						start =
							lit( ’ / ’ )
							>> -(+(~char_( "/?" )))
							>> -(’ / ’ >> +(~char_( "?" )))
							>> -(’?’ >> (query_pair % ’&’))
							;
						
						query_pair = +(~char_( ’ = ’ )) >> ’ = ’ >> +(~char_( ’&’ ));
					}


					qi::rule< Iterator, daw::nodepp::base::Url( ) > start;
					qi::rule< Iterator, std::pair<std::string, std::string>( ) > query_pair;
				};	// struct url_encoded_data_grammar

			} // namespace url_parser

			std::shared_ptr <base::Url>( base::data_t::iterator first, base::data_t::iterator, base::Url& parts ) {
				omd::http::request::detail::uri_grammar< Iterator > grammar;
				return boost::spirit::qi::parse( begin, end,
					grammar,
					parts );
			}
		} // namespace base
	} // namespace nodepp
} // namespace daw
