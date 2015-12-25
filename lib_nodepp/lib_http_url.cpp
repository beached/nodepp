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

#include "lib_http_url.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				UrlAuthInfo::UrlAuthInfo( ): 
					username( ), 
					password( ) {
					
					set_links( );
				}

				UrlAuthInfo::UrlAuthInfo( std::string UserName, std::string Password ): 
					username( std::move( UserName ) ), 
					password( std::move( Password ) ) {
					
					set_links( );
				}

				UrlAuthInfo::~UrlAuthInfo( ) { }

				void UrlAuthInfo::set_links( ) {
					this->reset_jsonlink( );
					this->link_string( "username", username );
					this->link_string( "password", password );
				}

				HttpUrl::HttpUrl( ):
					scheme( ),
					auth_info( ),
					host( ),
					port( 0 ),
					request( request ) {
					
					set_links( );
				}

				HttpUrl::~HttpUrl( ) { }

				void HttpUrl::set_links( ) {
					this->reset_jsonlink( );
					this->link_string( "scheme", scheme );
					this->link_object( "auth_info", auth_info );
					this->link_string( "host", host );
					this->link_integral( "port", port );
					this->link_object( "request", request );
				}
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw

