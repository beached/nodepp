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
#include <map>
#include <memory>
#include <string>


BOOST_FUSION_ADAPT_STRUCT(
	daw::nodepp::lib::http::HttpRequestLine,
	(daw::nodepp::lib::http::HttpRequestMethod, method)
	(std::string, url)
	(std::string, version)
	)

	BOOST_FUSION_ADAPT_STRUCT(
	daw::nodepp::lib::http::HttpClientRequest,
	(daw::nodepp::lib::http::HttpRequestLine, request)
	(daw::nodepp::lib::http::HttpClientRequest::headers_t, headers)
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
					struct method_parse_symbol_: qi::symbols < char, daw::nodepp::lib::http::HttpRequestMethod > {
						method_parse_symbol_( ) {
							add
								( "OPTIONS", daw::nodepp::lib::http::HttpRequestMethod::Options )
								("GET", daw::nodepp::lib::http::HttpRequestMethod::Get)
								("HEAD", daw::nodepp::lib::http::HttpRequestMethod::Head)
								("POST", daw::nodepp::lib::http::HttpRequestMethod::Post)
								("PUT", daw::nodepp::lib::http::HttpRequestMethod::Put)
								("DELETE", daw::nodepp::lib::http::HttpRequestMethod::Delete)
								("TRACE", daw::nodepp::lib::http::HttpRequestMethod::Trace)
								("CONNECT", daw::nodepp::lib::http::HttpRequestMethod::Connect)
								;
						}
					} method_parse_symbol;
					}	// namespace anonymous

					template <typename Iterator>
					struct parse_grammar: qi::grammar < Iterator, daw::nodepp::lib::http::HttpClientRequest( ) > {
						parse_grammar( ) : parse_grammar::base_type( message ) {
							url = +(~char_( ' ' ));	// not space
							http_version = lexeme["HTTP/" >> raw[int_ >> '.' >> int_]];
							crlf = lexeme[lit( '\x0d' ) >> lit( '\x0a' )];	// cr followed by newline

							request_line =
								method_parse_symbol >> ' '
								>> url >> ' '
								>> http_version
								>> crlf
								;

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
						qi::rule< Iterator, daw::nodepp::lib::http::HttpClientRequest( ) > message;

						qi::rule< Iterator, std::string( ) > http_version;
						qi::rule< Iterator, std::string( ) > url;
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
