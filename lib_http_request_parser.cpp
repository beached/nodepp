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

#include "lib_http_request_parser.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;

				//////////////////////////////////////////////////////////////////////////
				// Summary: symbol table to describe the valid request methods
				//

				BOOST_FUSION_ADAPT_STRUCT(
					daw::nodepp::lib::http::request::HttpRequestLine,
					(daw::nodepp::lib::http::request::HttpRequestMethod, method)
					(std::string, uri)
					(std::string, version)
					)

					BOOST_FUSION_ADAPT_STRUCT(
					daw::nodepp::lib::http::request::HttpClientRequest,
					(daw::nodepp::lib::http::request::HttpRequestLine, request)
					(daw::nodepp::lib::http::request::HttpClientRequest::headers_t, headers)
					)

				namespace {
					namespace phoenix = boost::phoenix;
					namespace spirit = boost::spirit;
					namespace qi = boost::spirit::qi;
					namespace ascii = boost::spirit::ascii;
					namespace fusion = boost::fusion;

					using namespace qi::labels;
					using qi::lit;
					using qi::lexeme;
					using qi::on_error;
					using qi::raw;
					using spirit::_val;
					using spirit::_1;
					using spirit::int_;
					using spirit::attr;
					using spirit::omit;
					using ascii::alnum;
					using ascii::alpha;
					using ascii::space;
					using ascii::char_;
					using phoenix::construct;
					using phoenix::val;

					struct method_parse_symbol_: qi::symbols < char, daw::nodepp::lib::http::request::HttpRequestMethod > {
						method_parse_symbol_( ) {
							add
								( "OPTIONS", daw::nodepp::lib::http::request::HttpRequestMethod::Options )
								("GET", daw::nodepp::lib::http::request::HttpRequestMethod::Get)
								("HEAD", daw::nodepp::lib::http::request::HttpRequestMethod::Head)
								("POST", daw::nodepp::lib::http::request::HttpRequestMethod::Post)
								("PUT", daw::nodepp::lib::http::request::HttpRequestMethod::Put)
								("DELETE", daw::nodepp::lib::http::request::HttpRequestMethod::Delete)
								("TRACE", daw::nodepp::lib::http::request::HttpRequestMethod::Trace)
								("CONNECT", daw::nodepp::lib::http::request::HttpRequestMethod::Connect)
								;
						}
					} method_parse_symbol


					template <typename Iterator, request::HttpClientRequest >
					struct parse_grammar: qi::grammar < Iterator, daw::nodepp::lib::http::request::HttpClientRequest( ) > {
						parse_grammar( ) : parse_grammar::base_type( message ) {
							uri = +(~char_( ' ' ));	// not space
							http_version = lexeme["HTTP/" >> raw[int_ >> '.' >> int_]];
							crlf = lexeme[lit( '\x0d' ) >> lit( '\x0a' )];	// cr followed by newline

							request_line =
								method_parse_symbol >> ' '
								>> uri >> ' '
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

							//field_name = token.alias(); +( ~seperator ); //char_( "A-Z" ) >> *char_( "a-zA-Z_-" );

							//field_value = *( alnum | lws );
						}

						qi::rule< Iterator, request::HttpClientRequest( ) > message;
						qi::rule< Iterator > crlf;
						qi::rule< Iterator, std::string( ) > http_version;
						qi::rule< Iterator, std::string( ) > uri;
						qi::rule< Iterator, std::pair<std::string, std::string>( ) > header_pair;
						qi::rule< Iterator, daw::nodepp::lib::http::request::HttpRequestLine( ) > request_line;
						qi::rule< Iterator, std::string( ) > field_value;
						qi::rule< Iterator, std::string( ) > token;
						qi::rule< Iterator > lws;
					};	// struct parse_grammer
				}	// namespace anonymous

				std::shared_ptr<daw::nodepp::lib::http::request::HttpClientRequest> parse_http_request( std::string::iterator first, std::string::iterator last ) {
					auto result = std::make_shared < daw::nodepp::lib::http::request::HttpClientRequest >( );
					using Iterator = unsigned char const *;
					parse_grammar<Iterator> grammar;
					if( !boost::spirit::qi::parse( first, last, grammar, *result ) ) {
						result = nullptr;
					}
					return result;

				}

			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
