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
	daw::nodepp::base::url_details::UserInfo,
	(std::string, username)
	(boost::optional<std::string>, password)
)

BOOST_FUSION_ADAPT_STRUCT(
	daw::nodepp::base::url_details::UrlAuthority,
	(boost::optional < daw::nodepp::base::url_details::UserInfo, user_info)
	(std::string, host)
	(boost::optional<std::string>, host)
)

BOOST_FUSION_ADAPT_STRUCT(
	daw::nodepp::base::Url,
	(boost::optional<std::string>, scheme)
	(daw::nodepp::base::url_details::UrlAuthority, authority)
	(boost::optional<std::string>, path)
	(boost::optional<daw::nodepp::base::Url::query_t>, queries)
	(boost::optional<std::string>, fragment)
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
				struct url_encoded_data_grammar: qi::grammar < Iterator, std::map<std::string, std::string>( ) > {
					url_encoded_data_grammar( ) : url_encoded_data_grammar::base_type( start ) {
						start = query_pair % '&';
						query_pair = url_encoded_string >> '=' >> url_encoded_string;
						url_encoded_string = +(('%' >> encoded_hex)
							|
							~char_( "=&" )
							);
					}

					qi::rule< Iterator, std::map<std::string, std::string>( ) > start;
					qi::rule< Iterator, std::pair<std::string, std::string>( ) > query_pair;
					qi::rule< Iterator, std::string( ) > url_encoded_string;
					qi::uint_parser< char, 16, 2, 2 > encoded_hex;
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
