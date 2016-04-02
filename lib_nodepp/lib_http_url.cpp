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

				std::string to_string( UrlAuthInfo const & auth ) {
					std::stringstream ss;
					ss <<auth.username <<":" <<auth.password;
					return ss.str( );
				}

				std::ostream& operator<<( std::ostream& os, UrlAuthInfo const & auth ) {
					os <<to_string( auth );
					return os;
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

				HttpUrlQueryPair::~HttpUrlQueryPair( ) { }

				void HttpUrlQueryPair::set_links( ) {
					this->reset_jsonlink( );
					this->link_string( "name", name );
					this->link_string( "value", value );
				}

				HttpAbsoluteUrlPath::HttpAbsoluteUrlPath( ):
					JsonLink( ),
					path( ),
					query( ),
					fragment( ) {
					set_links( );
				}

				HttpAbsoluteUrlPath::~HttpAbsoluteUrlPath( ) { }

				void HttpAbsoluteUrlPath::set_links( ) {
					this->reset_jsonlink( );

					this->link_string( "path", path );
					this->link_array( "query", query );
					this->link_string( "fragment", fragment );
				}

				std::string to_string( HttpAbsoluteUrlPath const & url_path ) {
					std::stringstream ss;
					ss <<url_path.path;
					if( url_path.query ) {
						for( auto const & qp : url_path.query.get( ) ) {
							ss <<"?" <<qp.name;
							if( qp.value ) {
								ss <<"=" <<qp.value.get( );
							}
						}
					}
					if( url_path.fragment ) {
						ss <<"#" <<url_path.fragment.get( );
					}
					return ss.str( );
				}

				std::ostream& operator<<( std::ostream& os, HttpAbsoluteUrlPath const & url_path ) {
					os <<to_string( url_path );
					return os;
				}

				void UrlAuthInfo::set_links( ) {
					this->reset_jsonlink( );
					this->link_string( "username", username );
					this->link_string( "password", password );
				}

				namespace impl {					
					HttpUrlImpl::HttpUrlImpl( ):
						scheme( ),
						auth_info( ),
						host( ),
						port( ),
						path( ) {
						
						set_links( );
					}
	
					HttpUrlImpl::~HttpUrlImpl( ) { }
	
					void HttpUrlImpl::set_links( ) {
						this->reset_jsonlink( );
						this->link_string( "scheme", scheme );
						this->link_string( "host", host );
						this->link_integral( "port", port );
						this->link_object( "auth_info", auth_info );
						this->link_object( "path", path );
					}
				}	// namespace impl
				
				std::string to_string( HttpUrl const & url ) {
					return url ? to_string( *url ) : "";
				}

				std::string to_string( impl::HttpUrlImpl const & url ) {
					std::stringstream ss;
					ss <<url.scheme <<"://";
					ss <<url.host;
					if( url.port ) {
						ss <<*(url.port);
					}

					if( url.auth_info ) {
						ss <<*(url.auth_info) <<"@";
					}

					if( url.path ) {
						ss <<"/" <<*(url.path);
					}

					return ss.str( );
				}

				std::ostream& operator<<( std::ostream& os, http::HttpUrl const & url ) {
					if( url ) {
						os <<to_string( *url );
					}
					return os;
				}

				std::ostream& operator<<( std::ostream& os, impl::HttpUrlImpl const & url ) {
					os <<to_string( url );
					return os;	
				}


			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw

