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
#include "parse_json/daw_json_link.h"

namespace daw {
	namespace nodepp {
		namespace base {
			namespace json {
				struct JsonLink;
			}
		}
		namespace lib {
			namespace http {
				enum class HttpClientRequestMethod { Options = 1, Get, Head, Post, Put, Delete, Trace, Connect, Any };

				std::ostream& operator<<(std::ostream& os, HttpClientRequestMethod const method);
				std::istream& operator>>(std::istream& is, HttpClientRequestMethod & method);
				std::string to_string( HttpClientRequestMethod method );

				std::string value_to_json( std::string const & name, HttpClientRequestMethod method );

				struct HttpUrlQueryPair: public daw::json::JsonLink < HttpUrlQueryPair > {
					std::string name;
					boost::optional<std::string> value;

					~HttpUrlQueryPair( ) = default;
					HttpUrlQueryPair( HttpUrlQueryPair const & ) = default;
					HttpUrlQueryPair& operator=(HttpUrlQueryPair const &) = default;

					HttpUrlQueryPair( );
					HttpUrlQueryPair( std::pair<std::string, boost::optional<std::string>> const & vals );
					HttpUrlQueryPair( HttpUrlQueryPair && other );
					HttpUrlQueryPair& operator=(HttpUrlQueryPair && rhs);
					void set_links( );
				};	// struct HttpUrlQueryPair

				struct HttpUrl: public daw::json::JsonLink < HttpUrl > {
					std::string path;
					boost::optional<std::vector<HttpUrlQueryPair>> query;
					boost::optional<std::string> fragment;

					~HttpUrl( ) = default;
					HttpUrl( HttpUrl const & ) = default;
					HttpUrl& operator=(HttpUrl const &) = default;

					HttpUrl( );
					HttpUrl( HttpUrl && other );
					HttpUrl& operator=(HttpUrl && rhs);
					void set_links( );
				};	// struct HttpUrl

				struct HttpRequestLine: public daw::json::JsonLink < HttpRequestLine > {
					HttpClientRequestMethod method;
					HttpUrl url;
					std::string version;

					~HttpRequestLine( ) = default;
					HttpRequestLine( HttpRequestLine const & ) = default;
					HttpRequestLine& operator=(HttpRequestLine const &) = default;

					HttpRequestLine( );
					HttpRequestLine( HttpRequestLine && other );
					HttpRequestLine& operator=(HttpRequestLine && rhs);
					void set_links( );
				};	// struct HttpRequestLine

				struct HttpClientRequestBody: public daw::json::JsonLink < HttpClientRequestBody > {
					std::string mime_type;
					std::string content;

					~HttpClientRequestBody( ) = default;
					HttpClientRequestBody( HttpClientRequestBody const & ) = default;
					HttpClientRequestBody& operator=(HttpClientRequestBody const &) = default;

					HttpClientRequestBody( );
					HttpClientRequestBody( HttpClientRequestBody && other );
					HttpClientRequestBody& operator=(HttpClientRequestBody && rhs);
					void set_links( );
				};	// struct HttpClientRequestBody

				struct HttpClientRequestHeaders: public daw::json::JsonLink < HttpClientRequestHeaders >, public daw::mixins::MapLikeProxy < HttpClientRequestHeaders, std::unordered_map< std::string, std::string> > {
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
					iterator find( boost::string_ref key ) {
						return headers.find( key.to_string( ) );
					}

					const_iterator find( boost::string_ref key ) const {
						return headers.find( key.to_string( ) );
					}
				};

				namespace impl {
					struct HttpClientRequestImpl: public daw::json::JsonLink < HttpClientRequestImpl > {
						using headers_t = std::unordered_map < std::string, std::string > ;
						daw::nodepp::lib::http::HttpRequestLine request_line;
						headers_t headers;
						boost::optional<daw::nodepp::lib::http::HttpClientRequestBody> body;

						~HttpClientRequestImpl( ) = default;
						HttpClientRequestImpl( HttpClientRequestImpl const & ) = default;
						HttpClientRequestImpl& operator=(HttpClientRequestImpl const &) = default;

						HttpClientRequestImpl( );
						HttpClientRequestImpl( HttpClientRequestImpl && other );
						HttpClientRequestImpl& operator=(HttpClientRequestImpl && rhs);
						void set_links( );
					};	// struct HttpClientRequestImpl
				}	// namespace impl

				// 				struct HttpClientRequest {
				// 					HttpClientRequestMethod method;
				// 					std::string version;
				// 					std::string path;
				// 					typename impl::HttpClientRequestImpl::headers_t headers;
				// 					boost::optional<std::vector<impl::HttpUrlQueryPairImpl>> query;
				// 				};	// struct HttpClientRequest

				using HttpClientRequest = std::shared_ptr < impl::HttpClientRequestImpl > ;

				HttpClientRequest parse_http_request( daw::nodepp::base::data_t::iterator first, daw::nodepp::base::data_t::iterator last );
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
