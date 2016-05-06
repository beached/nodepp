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

#include <boost/spirit/include/qi_parse_attr.hpp>
#include <ostream>

#include "base_types.h"
#include "lib_http_request.h"
#include <daw/parse_json/daw_json.h>
#include <daw/daw_utility.h>
#include "lib_http_parser.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				using namespace daw::nodepp::base::json;

				std::string to_string( HttpClientRequestMethod method ) {
					switch( method ) {
					case HttpClientRequestMethod::Get:
						return "GET";
					case HttpClientRequestMethod::Post:
						return "POST";
					case HttpClientRequestMethod::Connect:
						return "CONNECT";
					case HttpClientRequestMethod::Delete:
						return "DELETE";
					case HttpClientRequestMethod::Head:
						return "HEAD";
					case HttpClientRequestMethod::Options:
						return "OPTIONS";
					case HttpClientRequestMethod::Put:
						return "PUT";
					case HttpClientRequestMethod::Trace:
						return "TRACE";
					case HttpClientRequestMethod::Any:
						return "ANY";
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

				std::ostream& operator<<( std::ostream& os, HttpClientRequestMethod method ) {
					os <<to_string( method );
					return os;
				}

				std::istream& operator>>( std::istream& is, HttpClientRequestMethod & method ) {
					std::string method_string;
					is>> method_string;
					method = http_request_method_from_string( method_string );
					return is;
				}

				HttpRequestLine::HttpRequestLine( ):
					JsonLink( ),
					method( ),
					url( ),
					version( ) {
					set_links( );
				}

				HttpRequestLine::~HttpRequestLine( ) { }

				void HttpRequestLine::set_links( ) {
					this->reset_jsonlink( );
					this->link_streamable( "method", method );
					this->link_object( "url", url );
					this->link_string( "version", version );
				}

				HttpClientRequestBody::HttpClientRequestBody( ): content_type( ), content( ) {
					set_links( );
				}

				HttpClientRequestBody::~HttpClientRequestBody( ) { }

				void HttpClientRequestBody::set_links( ) {
					this->reset_jsonlink( );
					this->link_string( "content_type", content_type );
					this->link_string( "content", content );
				}

				HttpClientRequestHeaders::HttpClientRequestHeaders( HttpClientRequestHeaders::container_type h ): headers( std::move( h ) ) { }

				HttpClientRequestHeaders::~HttpClientRequestHeaders( ) { }

				HttpClientRequestHeaders::iterator HttpClientRequestHeaders::find( boost::string_ref key ) {
					return headers.find( key.to_string( ) );
				}

				HttpClientRequestHeaders::const_iterator HttpClientRequestHeaders::find( boost::string_ref key ) const {
					return headers.find( key.to_string( ) );
				}

				namespace impl {
					HttpClientRequestImpl::HttpClientRequestImpl( ):
						JsonLink( ),
						request_line( ),
						headers( ),
						body( ) {
						set_links( );
					}

					HttpClientRequestImpl::~HttpClientRequestImpl( ) { }

					void HttpClientRequestImpl::set_links( ) {
						this->reset_jsonlink( );
						this->link_object( "request", request_line );
						this->link_map( "headers", headers );
						this->link_object( "body", body );
					}
				}	// namespace impl

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

