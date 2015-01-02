#pragma once

#include <boost/config/warning_disable.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_grammar.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

BOOST_FUSION_ADAPT_STRUCT(
	daw::nodepp::lib::http::HttpUrl,
	(std::string, path )
	(boost::optional<std::string>, query)
	)

BOOST_FUSION_ADAPT_STRUCT(
	daw::nodepp::lib::http::HttpRequestLine,
	(daw::nodepp::lib::http::HttpClientRequestMethod, method)
	(daw::nodepp::lib::http::HttpUrl, url)
	(std::string, version)
	)

	BOOST_FUSION_ADAPT_STRUCT(
	daw::nodepp::lib::http::impl::HttpClientRequestImpl,
	(daw::nodepp::lib::http::HttpRequestLine, request)
	(daw::nodepp::lib::http::impl::HttpClientRequestImpl::headers_t, headers)
	)


namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				namespace request_parser {
					using namespace daw::nodepp;

					namespace ascii = boost::spirit::ascii;
					namespace qi = boost::spirit::qi;
					namespace spirit = boost::spirit;

					using ascii::char_;
					using qi::lexeme;
					using qi::lit;
					using qi::raw;
					using spirit::int_;
					using spirit::omit;
					namespace {
					struct method_parse_symbol_: qi::symbols < char, daw::nodepp::lib::http::HttpClientRequestMethod > {
						method_parse_symbol_( ) {
							add
								("OPTIONS", daw::nodepp::lib::http::HttpClientRequestMethod::Options)
								("GET", daw::nodepp::lib::http::HttpClientRequestMethod::Get)
								("HEAD", daw::nodepp::lib::http::HttpClientRequestMethod::Head)
								("POST", daw::nodepp::lib::http::HttpClientRequestMethod::Post)
								("PUT", daw::nodepp::lib::http::HttpClientRequestMethod::Put)
								("DELETE", daw::nodepp::lib::http::HttpClientRequestMethod::Delete)
								("TRACE", daw::nodepp::lib::http::HttpClientRequestMethod::Trace)
								("CONNECT", daw::nodepp::lib::http::HttpClientRequestMethod::Connect)
								;
						}
					} method_parse_symbol;
					}	// namespace anonymous

// 					template <typename Iterator >
// 					struct uri_encoded_data_grammar: qi::grammar < Iterator, std::map<std::string, std::string>( ) > {
// 						uri_encoded_data_grammar( ) : uri_encoded_data_grammar::base_type( start ) {
// 							start = query_pair % '&';
// 							query_pair = url_encoded_string >> '=' >> url_encoded_string;
// 							url_encoded_string = +(('%' >> encoded_hex)
// 								|
// 								~char_( "=&" )
// 								);
// 						}
// 						qi::rule< Iterator, std::map<std::string, std::string>( ) > start;
// 						qi::rule< Iterator, std::pair<std::string, std::string>( ) > query_pair;
// 						qi::rule< Iterator, std::string( ) > url_encoded_string;
// 						qi::uint_parser< char, 16, 2, 2 > encoded_hex;
// 					};

// 					template <typename Iterator>
// 					struct url_query_parse_grammar: qi::grammar < Iterator, boost::optional<std::string>( ) > {
// 						url_query_parse_grammar( ) : url_query_parse_grammar::base_type( query ) {
// 							query = query_pair % query_separator;
// 							query_pair = query_key >> -('=' >> -query_value);							
// 							query_key = char_( "a-zA-Z_" ) >> *char_( "a-zA-Z_0-9" );
// 							query_value = +char_( "a-zA-Z_0-9" );
// 							query_separator = lit( ';' ) | '&';
// 						}
// 
// 						qi::rule< Iterator> query_separator;
// 						qi::rule < Iterator, std::string( )> query_key;
// 						qi::rule < Iterator, std::string( )> query_value;
// 						
// 					//	qi::rule< Iterator, daw::nodepp::lib::http::HttpUrl::query_pair_t( )> query_pair;
// 						qi::rule< Iterator, boost::optional<std::string>( )> query;
// 						
// 					};

					template <typename Iterator>
					struct abs_url_parse_grammar: qi::grammar < Iterator, daw::nodepp::lib::http::HttpUrl( ) > {
						abs_url_parse_grammar( ) : abs_url_parse_grammar::base_type( url ) { 
							url = path >> -('?' >> query);
						}
						qi::rule< Iterator, daw::nodepp::lib::http::HttpUrl( ) > url;
						qi::rule< Iterator, std::string( ) > path;
						qi::rule< Iterator, boost::optional<std::string>> query;
						//url_query_parse_grammar<Iterator> query;
					};

					template <typename Iterator>
					struct http_request_parse_grammar: qi::grammar < Iterator, daw::nodepp::lib::http::impl::HttpClientRequestImpl( ) > {
						http_request_parse_grammar( ) : http_request_parse_grammar::base_type( message ) {
							request_line =
								method_parse_symbol >> ' '
								>> url >> ' '
								>> http_version
								>> crlf
								;

							http_version = lexeme["HTTP/" >> raw[int_ >> '.' >> int_]];
							crlf = lexeme[lit( '\x0d' ) >> lit( '\x0a' )];	// cr followed by newline
							

							token = +(~char_( "()<>@,;:\\\"/[]?={} \x09" ));
							lws = omit[-crlf >> *char_( " \x09" )];
							field_value = *(char_ - crlf);
							header_pair = token >> ':' >> lws >> field_value >> crlf;

							message =
								request_line
								>> *header_pair
								>> crlf
								;
						}

						qi::rule< Iterator > crlf;
						qi::rule< Iterator, daw::nodepp::lib::http::impl::HttpClientRequestImpl( ) > message;

						qi::rule< Iterator, std::string( ) > http_version;
						abs_url_parse_grammar<Iterator> url;
						qi::rule< Iterator, std::pair<std::string, std::string>( ) > header_pair;
						qi::rule< Iterator, daw::nodepp::lib::http::HttpRequestLine( ) > request_line;
						qi::rule< Iterator, std::string( ) > field_value;
						qi::rule< Iterator, std::string( ) > token;
						qi::rule< Iterator > lws;
					};	// struct parse_grammer
				}	// namespace request_parser
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
