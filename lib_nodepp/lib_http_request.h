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

#include <boost/optional.hpp>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "base_types.h"
#include <daw/parse_json/daw_json_link.h>
#include "lib_http_url.h"
#include "lib_http_parser.h"

namespace daw {
	namespace nodepp {
		namespace base {
			namespace json {
				struct JsonLink;
			}	// namespace json
		}	// namespace base
		namespace lib {
			namespace http {
				enum class HttpClientRequestMethod { Options = 1, Get, Head, Post, Put, Delete, Trace, Connect, Any };

				std::ostream& operator<<( std::ostream& os, HttpClientRequestMethod const method );
				std::istream& operator>>( std::istream& is, HttpClientRequestMethod & method );
				std::string to_string( HttpClientRequestMethod method );

				std::string value_to_json( std::string const & name, HttpClientRequestMethod method );
				

				struct HttpRequestLine final: public daw::json::JsonLink <HttpRequestLine> {
					HttpClientRequestMethod method;
					HttpAbsoluteUrlPath url;
					std::string version;

					HttpRequestLine( );
					~HttpRequestLine( );
					HttpRequestLine( HttpRequestLine const & ) = default;
					HttpRequestLine( HttpRequestLine && ) = default;
					HttpRequestLine& operator=( HttpRequestLine const & ) = default;
					HttpRequestLine& operator=( HttpRequestLine && ) = default;

					void set_links( );
				};	// struct HttpRequestLine

				struct HttpClientRequestBody: public daw::json::JsonLink <HttpClientRequestBody> {
					std::string content_type;
					std::string content;

					HttpClientRequestBody( );
					~HttpClientRequestBody( );
					HttpClientRequestBody( HttpClientRequestBody const & ) = default;
					HttpClientRequestBody( HttpClientRequestBody && ) = default;
					HttpClientRequestBody& operator=( HttpClientRequestBody const & ) = default;
					HttpClientRequestBody& operator=( HttpClientRequestBody && ) = default;
					
					void set_links( );
				};	// struct HttpClientRequestBody

				struct HttpClientRequestHeaders final: public daw::json::JsonLink <HttpClientRequestHeaders>, public daw::mixins::MapLikeProxy <HttpClientRequestHeaders, std::unordered_map<std::string, std::string>> {
					container_type headers;
					using key_type = std::string;
					using mapped_type = std::string;
				protected:
					inline container_type & container( ) {
						return headers;
					}

					inline container_type const & container( ) const {
						return headers;
					}
				public:
					HttpClientRequestHeaders( ) = default;
					explicit HttpClientRequestHeaders( container_type h );
					~HttpClientRequestHeaders( );
					HttpClientRequestHeaders( HttpClientRequestHeaders const & ) = default;
					HttpClientRequestHeaders( HttpClientRequestHeaders && ) = default;
					HttpClientRequestHeaders & operator=( HttpClientRequestHeaders const & ) = default;
					HttpClientRequestHeaders & operator=( HttpClientRequestHeaders && ) = default;

					iterator find( boost::string_ref key );
					const_iterator find( boost::string_ref key ) const;
				};

				namespace impl {
					struct HttpClientRequestImpl final: public daw::json::JsonLink <HttpClientRequestImpl> {
						using headers_t = std::unordered_map <std::string, std::string>;
						daw::nodepp::lib::http::HttpRequestLine request_line;
						headers_t headers;
						boost::optional<daw::nodepp::lib::http::HttpClientRequestBody> body;

						HttpClientRequestImpl( );
						~HttpClientRequestImpl( );
						HttpClientRequestImpl( HttpClientRequestImpl const & ) = default;
						HttpClientRequestImpl( HttpClientRequestImpl && ) = default;
						HttpClientRequestImpl& operator=( HttpClientRequestImpl const & ) = default;
						HttpClientRequestImpl& operator=( HttpClientRequestImpl && ) = default;
						
						void set_links( );
					};	// struct HttpClientRequestImpl
				}	// namespace impl			
				
				HttpClientRequest create_http_client_request( boost::string_ref path, HttpClientRequestMethod const & method );
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw

