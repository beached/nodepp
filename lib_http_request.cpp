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

#include <boost/spirit/include/qi_parse_attr.hpp>
#include <ostream>

#include "base_types.h"
#include "lib_http_request.h"
#include "parse_json/daw_json.h"
#include "daw_utility.h"
#include "lib_http_request_parser_impl.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				using namespace daw::nodepp::base::json;

				std::string to_string( HttpClientRequestMethod method ) {
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

				std::string value_to_json( std::string const & name, HttpClientRequestMethod method ) {
					return daw::json::generate::value_to_json( name, to_string( method ) );
				}

				HttpClientRequestMethod http_request_method_from_string( std::string method ) {
					method = daw::AsciiLower( method );
					if( "get" == method ) {
						return HttpClientRequestMethod::Get;
					} else if( "post" == method ) {
						return HttpClientRequestMethod::Post;
					} else if( "connect" == method ) {
						return HttpClientRequestMethod::Connect;
					} else if( "delete" == method ) {
						return HttpClientRequestMethod::Delete;
					} else if( "head" == method ) {
						return HttpClientRequestMethod::Head;
					} else if( "options" == method ) {
						return HttpClientRequestMethod::Options;
					} else if( "put" == method ) {
						return HttpClientRequestMethod::Put;
					} else if( "trace" == method ) {
						return HttpClientRequestMethod::Trace;
					} else if( "any" == method ) {
						return HttpClientRequestMethod::Any;
					}
					throw std::runtime_error( "unknown http request method" );
				}

				std::ostream& operator<<( std::ostream& os, HttpClientRequestMethod const method ) {
					os << to_string( method );
					return os;
				}

				std::istream& operator>>( std::istream& is, HttpClientRequestMethod & method ) {
					std::string method_string;
					is >> method_string;
					method = http_request_method_from_string( method_string );
					return is;
				}

				std::string to_string( HttpUrl const & url ) {
					std::stringstream ss;
					ss << url.path;
					if( url.query ) {
						for( auto const & qp : url.query.get( ) ) {
							ss << "?" << qp.name;
							if( qp.value ) {
								ss << "=" << qp.value.get( );
							}
						}
					}
					if( url.fragment ) {
						ss << "#" << url.fragment.get( );
					}
					return ss.str( );
				}

				HttpUrlQueryPair::HttpUrlQueryPair( ):
					JsonLink( ),
					name( ),
					value( ) {
					set_links( );
				}

				HttpUrlQueryPair::HttpUrlQueryPair( std::pair<std::string, boost::optional<std::string>> const & vals ):
					JsonLink( ),
					name( vals.first ),
					value( vals.second ) {
					set_links( );
				}

				HttpUrlQueryPair::HttpUrlQueryPair( HttpUrlQueryPair && other ):
					JsonLink( std::move( other ) ),
					name( std::move( other.name ) ),
					value( std::move( other.value ) ) {
					set_links( );
				}

				HttpUrlQueryPair& HttpUrlQueryPair::operator=( HttpUrlQueryPair && rhs ) {
					if( this != &rhs ) {
						name = std::move( rhs.name );
						value = std::move( rhs.value );
						set_links( );
					}
					return *this;
				}

				void HttpUrlQueryPair::set_links( ) {
					this->reset_jsonlink( );
					this->link_string( "name", name );
					this->link_string( "value", value );
				}

				HttpUrl::HttpUrl( ):
					JsonLink( ),
					path( ),
					query( ),
					fragment( ) {
					set_links( );
				}

				HttpUrl::HttpUrl( HttpUrl && other ):
					JsonLink( std::move( other ) ),
					path( std::move( other.path ) ),
					query( std::move( other.query ) ),
					fragment( std::move( other.fragment ) ) {
					set_links( );
				}

				HttpUrl& HttpUrl::operator=( HttpUrl && rhs ) {
					if( this != &rhs ) {
						path = std::move( rhs.path );
						query = std::move( rhs.query );
						fragment = std::move( rhs.fragment );
						set_links( );
					}
					return *this;
				}

				void HttpUrl::set_links( ) {
					this->reset_jsonlink( );

					this->link_string( "path", path );
					this->link_array( "query", query );
					this->link_string( "fragment", fragment );
				}

				HttpRequestLine::HttpRequestLine( ):
					JsonLink( ),
					method( ),
					url( ),
					version( ) {
					set_links( );
				}

				HttpRequestLine::HttpRequestLine( HttpRequestLine && other ):
					JsonLink( std::move( other ) ),
					method( std::move( other.method ) ),
					url( std::move( other.url ) ),
					version( std::move( other.version ) ) {
					set_links( );
				}

				HttpRequestLine& HttpRequestLine::operator=( HttpRequestLine && rhs ) {
					if( this != &rhs ) {
						method = std::move( rhs.method );
						url = std::move( rhs.url );
						version = std::move( rhs.version );
						set_links( );
					}
					return *this;
				}

				void HttpRequestLine::set_links( ) {
					this->reset_jsonlink( );
					this->link_streamable( "method", method );
					this->link_object( "url", url );
					this->link_string( "version", version );
				}

				HttpClientRequestBody::HttpClientRequestBody( ): content_type( ), content( ) {
					set_links( );
				}

				HttpClientRequestBody::HttpClientRequestBody( HttpClientRequestBody && other ):
					JsonLink<HttpClientRequestBody>( std::move( other ) ),
					content_type( std::move( other.content_type ) ),
					content( std::move( other.content ) ) {
					set_links( );
				}

				HttpClientRequestBody& HttpClientRequestBody::operator=( HttpClientRequestBody && rhs ) {
					if( this != &rhs ) {
						content_type = std::move( rhs.content_type );
						content = std::move( rhs.content );
						set_links( );
					}
					return *this;
				}

				void HttpClientRequestBody::set_links( ) {
					this->reset_jsonlink( );
					this->link_string( "content_type", content_type );
					this->link_string( "content", content );
				}

				namespace impl {
					HttpClientRequestImpl::HttpClientRequestImpl( ):
						JsonLink( ),
						request_line( ),
						headers( ),
						body( ) {
						set_links( );
					}

					HttpClientRequestImpl::HttpClientRequestImpl( HttpClientRequestImpl && other ):
						JsonLink( std::move( other ) ),
						request_line( std::move( other.request_line ) ),
						headers( std::move( other.headers ) ),
						body( std::move( other.body ) ) {
						set_links( );
					}

					HttpClientRequestImpl& HttpClientRequestImpl::operator=( HttpClientRequestImpl && rhs ) {
						if( this != &rhs ) {
							request_line = std::move( rhs.request_line );
							headers = std::move( rhs.headers );
							body = std::move( rhs.body );
							set_links( );
						}
						return *this;
					}

					void HttpClientRequestImpl::set_links( ) {
						this->reset_jsonlink( );
						this->link_object( "request", request_line );
						this->link_map( "headers", headers );
						this->link_object( "body", body );
					}
				}	// namespace impl

				std::shared_ptr<daw::nodepp::lib::http::HttpUrl> parse_url_path( boost::string_ref path ) {
					auto result = std::make_shared < daw::nodepp::lib::http::HttpUrl>( );
					if( !boost::spirit::qi::parse( path.begin( ), path.end( ), daw::nodepp::lib::http::request_parser::abs_url_parse_grammar<decltype(path.begin( ))>( ), *result ) ) {
						result = nullptr;
					}
					return result;
				}

				HttpClientRequest create_http_client_request( boost::string_ref path, HttpClientRequestMethod const & method ) {
					auto result = std::make_shared<impl::HttpClientRequestImpl>( );
					result->request_line.method = method;
					auto url = parse_url_path( path );
					if( url ) {
						result->request_line.url = *url;
					}
					return result;
				}
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw