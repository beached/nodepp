// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Darrell Wright
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

#pragma once

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
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "lib_http_request.h"
#include "lib_http_url.h"

	BOOST_FUSION_ADAPT_STRUCT(
	daw::nodepp::lib::http::HttpUrlQueryPair,
	(std::string, name)
	(boost::optional<std::string>, value)
	)

	BOOST_FUSION_ADAPT_STRUCT(
	daw::nodepp::lib::http::HttpAbsoluteUrlPath,
	(std::string, path)
	(boost::optional<std::vector<daw::nodepp::lib::http::HttpUrlQueryPair>>, query)
	(boost::optional<std::string>, fragment)
	)

	BOOST_FUSION_ADAPT_STRUCT(
	daw::nodepp::lib::http::UrlAuthInfo,
	(std::string, username)
	(std::string, password)
	)

	BOOST_FUSION_ADAPT_STRUCT(
	daw::nodepp::lib::http::impl::HttpUrlImpl,
	(std::string, scheme)
	(boost::optional<daw::nodepp::lib::http::UrlAuthInfo>, auth_info)
	(std::string, host)
	(boost::optional<uint16_t>, port)
	(boost::optional<daw::nodepp::lib::http::HttpAbsoluteUrlPath>, path)
	)

	BOOST_FUSION_ADAPT_STRUCT(
	daw::nodepp::lib::http::HttpRequestLine,
	(daw::nodepp::lib::http::HttpClientRequestMethod, method)
	(daw::nodepp::lib::http::HttpAbsoluteUrlPath, url)
	(std::string, version)
	)

	BOOST_FUSION_ADAPT_STRUCT(
	daw::nodepp::lib::http::impl::HttpClientRequestImpl,
	(daw::nodepp::lib::http::HttpRequestLine, request_line)
	(daw::nodepp::lib::http::impl::HttpClientRequestImpl::headers_t, headers)
	)

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				namespace request_parser {
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
						struct method_parse_symbol_: qi::symbols <char, daw::nodepp::lib::http::HttpClientRequestMethod> {
							method_parse_symbol_( ) {
								add
									( "OPTIONS", daw::nodepp::lib::http::HttpClientRequestMethod::Options )
									("GET", daw::nodepp::lib::http::HttpClientRequestMethod::Get)
									("HEAD", daw::nodepp::lib::http::HttpClientRequestMethod::Head)
									("POST", daw::nodepp::lib::http::HttpClientRequestMethod::Post)
									("PUT", daw::nodepp::lib::http::HttpClientRequestMethod::Put)
									("DELETE", daw::nodepp::lib::http::HttpClientRequestMethod::Delete)
									("TRACE", daw::nodepp::lib::http::HttpClientRequestMethod::Trace)
									("CONNECT", daw::nodepp::lib::http::HttpClientRequestMethod::Connect)
									;

								name( "method_parse_symbol" );
							}
						};	// struct method_parse_symbol_
					}	// namespace anonymous

					template <typename Iterator>
					struct abs_url_parse_grammar: qi::grammar <Iterator, daw::nodepp::lib::http::HttpAbsoluteUrlPath( )> {
						abs_url_parse_grammar( ): abs_url_parse_grammar::base_type( url ) {
							key = qi::char_( "a-zA-Z_" )>> *qi::char_( "a-zA-Z_0-9" );
							value = +qi::char_( "a-zA-Z_0-9" );
							path = char_( '/' )>> *(~char_( " ?#" ));
							query_pair = key>> -('='>> value);
							query = lit( '?' )>> query_pair>> *((qi::lit( ';' ) | '&')>> query_pair);
							fragment = lit( '#' )>> *(~char_( " " ));

							url = qi::eps> path>> -query>> -fragment;

							url.name( "url" );
							path.name( "path" );
							query.name( "query" );
							query_pair.name( "query_pair" );
							key.name( "key" );
							value.name( "value" );
							fragment.name( "fragment" );
							using namespace qi::labels;
							using namespace boost::phoenix;
							using qi::fail;
							using qi::debug;

							qi::on_error <fail>( url,
												   boost::phoenix::ref( std::cout )
												   <<val( "Error! Expecting " )
												   <<qi::_4
												   <<val( " here: \"" )
												   <<construct<std::string>( qi::_3, qi::_2 )
												   <<val( "\"" )
												   <<std::endl
												   );

											   //debug( url );
						}
						qi::rule<Iterator, daw::nodepp::lib::http::HttpAbsoluteUrlPath( )> url;
						qi::rule<Iterator, std::string( )> path;
						qi::rule<Iterator, daw::nodepp::lib::http::HttpUrlQueryPair( )> query_pair;
						qi::rule<Iterator, boost::optional<std::vector<daw::nodepp::lib::http::HttpUrlQueryPair>>( )> query;
						qi::rule<Iterator, std::string( )> key;
						qi::rule<Iterator, std::string( )> value;
						qi::rule<Iterator, std::string( )> fragment;
					};	// struct abs_url_parse_grammar

					template<typename Iterator>
					struct url_parse_grammar: qi::grammar<Iterator, daw::nodepp::lib::http::impl::HttpUrlImpl( )> {
						url_parse_grammar( ): url_parse_grammar::base_type( url_string ) {
							scheme = qi::char_( "a-zA-Z" )>> *qi::char_( "a-zA-Z_0-9+.-" );
							username = +qi::char_( "a-zA-Z_0-9+.-" );
							password = +qi::char_( "a-zA-Z_0-9+.-" );
							auth_info = username>> lit( ':' )>> password>> lit( '@' );
							port = lit( ':' )>> qi::uint_;
							host = +(~char_( "()<>@,;:\\\"/[]?={} \x09" ));
							url_string = qi::eps> scheme>> lit( "://" )>> -auth_info>> host>> -port>> -path;

							scheme.name( "scheme" );
							username.name( "username" );
							password.name( "password" );
							auth_info.name( "auth_info" );
							port.name( "port" );
							host.name( "host" );
							url_string.name( "url_string" );							
							path.name( "path" );

							using namespace qi::labels;
							using namespace boost::phoenix;
							using qi::fail;
							using qi::debug;

							qi::on_error <fail>( url_string,
												 boost::phoenix::ref( std::cout )
												 <<val( "Error! Expecting " )
												 <<qi::_4
												 <<val( " here: \"" )
												 <<construct<std::string>( qi::_3, qi::_2 )
												 <<val( "\"" )
												 <<std::endl
												 );

							//debug( url );

						}

						qi::rule<Iterator, std::string( )> scheme;
						qi::rule<Iterator, std::string( )> username;
						qi::rule<Iterator, std::string( )> password;
						qi::rule<Iterator, daw::nodepp::lib::http::UrlAuthInfo( )> auth_info;
						qi::rule<Iterator, uint16_t> port;
						qi::rule<Iterator, std::string( )> host;
						qi::rule<Iterator, daw::nodepp::lib::http::impl::HttpUrlImpl( )> url_string;
						abs_url_parse_grammar<Iterator> path;
//						qi::rule<Iterator, daw::nodepp::lib::http::HttpAbsoluteUrlPath( )> path;
					};	// struct url_parse_grammar

					template <typename Iterator>
					struct http_request_parse_grammar: qi::grammar <Iterator, daw::nodepp::lib::http::impl::HttpClientRequestImpl( )> {
						http_request_parse_grammar( ): http_request_parse_grammar::base_type( message ) {
							http_version = lexeme["HTTP/">> raw[int_>> '.'>> int_]];
							crlf = lexeme[lit( '\x0d' )>> lit( '\x0a' )];	// cr followed by newline
							//crlf = lexeme[-(lit( "\\r" ))>> lit( "\\n" )];

							token = +(~char_( "()<>@,;:\\\"/[]?={} \x09" ));
							lws = omit[-crlf>> *char_( " \x09" )];
							field_value = *(char_ - crlf);
							header_pair = token>> ':'>> lws>> field_value>> crlf;

							request_line =
								method_parse_symbol>> ' '
								>> url>> ' '
								>> http_version
								>> crlf
								;

							message = qi::eps>
								request_line
								>> *header_pair
								>> crlf
								;

							using namespace qi::labels;
							using namespace boost::phoenix;
							using qi::fail;
							using qi::debug;

							message.name( "message" );
							http_version.name( "http_version" );
							url.name( "url" );
							header_pair.name( "header_pair" );
							request_line.name( "request_line" );
							field_value.name( "field_value" );
							token.name( "token" );
							lws.name( "lws" );
							crlf.name( "newline" );

							qi::on_error <fail>( message,
												   boost::phoenix::ref( std::cout )
												   <<val( "Error! Expecting " )
												   <<qi::_4
												   <<val( " here: \"" )
												   <<construct<std::string>( qi::_3, qi::_2 )
												   <<val( "\"" )
												   <<std::endl
												   );

											   //debug( message );
						}
						method_parse_symbol_ method_parse_symbol;
						qi::rule<Iterator> crlf;
						qi::rule<Iterator, daw::nodepp::lib::http::impl::HttpClientRequestImpl( )> message;

						qi::rule<Iterator, std::string( )> http_version;
						abs_url_parse_grammar<Iterator> url;
						//qi::rule<Iterator, daw::nodepp::lib::http::HttpAbsoluteUrlPath( )> url;
						qi::rule<Iterator, std::pair<std::string, std::string>( )> header_pair;
						qi::rule<Iterator, daw::nodepp::lib::http::HttpRequestLine( )> request_line;
						qi::rule<Iterator, std::string( )> field_value;
						qi::rule<Iterator, std::string( )> token;
						qi::rule<Iterator> lws;
					};	// struct http_request_parse_grammer
				}	// namespace request_parser
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw

