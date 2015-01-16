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
#include "utility.h"

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
					return daw::json::value_to_json( name, to_string( method ) );
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

				std::ostream& operator<<(std::ostream& os, HttpClientRequestMethod const method) {
					os << to_string( method );
					return os;
				}

				std::istream& operator>>(std::istream& is, HttpClientRequestMethod & method) {
					std::string method_string;
					is >> method_string;
					method = http_request_method_from_string( method_string );
					return is;
				}

				HttpUrlQueryPair::HttpUrlQueryPair( ): 
					JsonLink( ), 
					name( ), 
					value( ) { 
					
					set_links( );
				}

				HttpUrlQueryPair::HttpUrlQueryPair( std::pair<std::string, boost::optional<std::string>> const & vals ) : 
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

				HttpUrlQueryPair& HttpUrlQueryPair::operator=(HttpUrlQueryPair && rhs) {
					if( this != &rhs ) {
						name = std::move( rhs.name );
						value = std::move( rhs.value );
						set_links( );
					}
					return *this;
				}

				void HttpUrlQueryPair::set_links( ) { 
					link_string( "name", name );
					link_string( "value", value );
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
					path(std::move( other.path ) ),
					query( std::move( other.query ) ),
					fragment( std::move( other.fragment ) ) {

					set_links( );
				}

				HttpUrl& HttpUrl::operator=(HttpUrl && rhs) {
					if( this != &rhs ) {
						path = std::move( rhs.path );
						query = std::move( rhs.query );
						fragment = std::move( rhs.fragment );
						set_links( );
					}
					return *this;
				}

				void HttpUrl::set_links( ) {
					link_string( "path", path );
					link_array( "query", query );
					link_string( "fragment", fragment );
				}

				HttpRequestLine::HttpRequestLine( ) :
					JsonLink( ),
					method( ),
					url( ),
					version( ) { 
				
					set_links( );
				}

				HttpRequestLine::HttpRequestLine( HttpRequestLine && other ) :
					JsonLink( std::move( other ) ),
					method( std::move( other.method ) ),
					url( std::move( other.url ) ),
					version( std::move( other.version ) ) {

					set_links( );
				}

				HttpRequestLine& HttpRequestLine::operator=(HttpRequestLine && rhs) {
					if( this != &rhs ) {
						method = std::move( rhs.method );
						url = std::move( rhs.url );
						version = std::move( rhs.version );
						set_links( );
					}
					return *this;
				}

				void HttpRequestLine::set_links( ) {
					link_streamable<decltype(method)>( "method", method );
					link_object( "url", url );
					link_string( "version", version );
				}

				namespace impl {
					HttpClientRequestImpl::HttpClientRequestImpl( ):
						JsonLink( ),
						request( ),
						headers( ) {

						set_links( );
					}

					HttpClientRequestImpl::HttpClientRequestImpl( HttpClientRequestImpl && other ):
						JsonLink( std::move( other ) ),
						request( std::move( other.request ) ),
						headers( std::move( other.headers ) ) {

						set_links( );
					}

					HttpClientRequestImpl& HttpClientRequestImpl::operator=(HttpClientRequestImpl && rhs) {
						if( this != &rhs ) {
							request = std::move( rhs.request );
							headers = std::move( rhs.headers );
							set_links( );
						}
						return *this;
					}

					void HttpClientRequestImpl::set_links( ) {
						link_object( "request", request );
						link_map( "headers", headers );
					}
				}	// namespace impl

			} // namespace http
		}	// namespace lib
	
	}	// namespace nodepp
}	// namespace daw


