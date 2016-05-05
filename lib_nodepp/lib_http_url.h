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

#include <string>
#include <memory>
#include <boost/utility/string_ref.hpp>
#include <boost/optional.hpp>
#include "base_types.h"
#include <daw/parse_json/daw_json_link.h>
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
				struct UrlAuthInfo: public daw::json::JsonLink<UrlAuthInfo> {
					std::string username;
					std::string password;

					UrlAuthInfo( );
					UrlAuthInfo( std::string UserName, std::string Password );
					~UrlAuthInfo( );
					UrlAuthInfo( UrlAuthInfo const & ) = default;
					UrlAuthInfo( UrlAuthInfo && ) = default;
					UrlAuthInfo & operator=( UrlAuthInfo const & ) = default;
					UrlAuthInfo & operator=( UrlAuthInfo && ) = default;
					void set_links( );
				};	// struct UrlAuthInfo

				std::string to_string( UrlAuthInfo const & auth );
				std::ostream& operator<<( std::ostream& os, UrlAuthInfo const & auth );
	
				struct HttpUrlQueryPair: public daw::json::JsonLink <HttpUrlQueryPair> {
					std::string name;
					boost::optional<std::string> value;

					HttpUrlQueryPair( );
					HttpUrlQueryPair( std::pair<std::string, boost::optional<std::string>> const & vals );
					~HttpUrlQueryPair( );
					HttpUrlQueryPair( HttpUrlQueryPair const & ) = default;
					HttpUrlQueryPair( HttpUrlQueryPair && ) = default;
					HttpUrlQueryPair& operator=( HttpUrlQueryPair const & ) = default;
					HttpUrlQueryPair& operator=( HttpUrlQueryPair && ) = default;

					void set_links( );
				};	// struct HttpUrlQueryPair

				struct HttpAbsoluteUrlPath: public daw::json::JsonLink<HttpAbsoluteUrlPath> {
					std::string path;
					boost::optional<std::vector<HttpUrlQueryPair>> query;
					boost::optional<std::string> fragment;

					HttpAbsoluteUrlPath( );
					~HttpAbsoluteUrlPath( );
					HttpAbsoluteUrlPath( HttpAbsoluteUrlPath const & ) = default;
					HttpAbsoluteUrlPath( HttpAbsoluteUrlPath && ) = default;
					HttpAbsoluteUrlPath& operator=( HttpAbsoluteUrlPath const & ) = default;
					HttpAbsoluteUrlPath& operator=( HttpAbsoluteUrlPath && ) = default;

					void set_links( );
				};	// struct HttpAbsoluteUrl

				std::string to_string( HttpAbsoluteUrlPath const & url_path );
				std::ostream& operator<<( std::ostream& os, HttpAbsoluteUrlPath const & url_path );

				namespace impl {
					struct HttpUrlImpl: public daw::json::JsonLink <HttpUrlImpl> {
						std::string scheme;
						boost::optional<UrlAuthInfo> auth_info;
						std::string host;
						boost::optional<uint16_t> port;
						boost::optional<HttpAbsoluteUrlPath> path;

						HttpUrlImpl( );
						~HttpUrlImpl( );

						HttpUrlImpl( HttpUrlImpl const & ) = default;
						HttpUrlImpl( HttpUrlImpl && ) = default;
						HttpUrlImpl& operator=( HttpUrlImpl const & ) = default;
						HttpUrlImpl& operator=( HttpUrlImpl && ) = default;

						void set_links( );
					};	// struct HttpUrlImpl
				}	// namespace impl

				std::string to_string( impl::HttpUrlImpl const & url );
				std::string to_string( HttpUrl const & url );
				std::ostream& operator<<( std::ostream& os, HttpUrl const & url );
				std::ostream& operator<<( std::ostream& os, impl::HttpUrlImpl const & url );

			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw


