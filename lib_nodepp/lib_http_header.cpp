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

#include <string>
#include <vector>

#include "lib_http_headers.h"
#include <daw/daw_string.h>

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				HttpHeader::HttpHeader( ) : name { }, value { } { }

				HttpHeader::HttpHeader( std::string Name, std::string Value ) : name( std::move( Name ) ), value( std::move( Value ) ) { }
				HttpHeader::HttpHeader( boost::string_ref Name, boost::string_ref Value ) : name( Name.to_string( ) ), value( Value.to_string( ) ) { }

				std::string HttpHeader::to_string( ) const {
					return name + ": " + value;
					//return daw::string::fmt( "{0}: {1}", name, value );
				}

				bool HttpHeader::empty( ) const {
					return name.empty( );
				}

				std::vector<HttpHeader>::iterator HttpHeaders::begin( ) {
					return headers.begin( );
				}

				std::vector<HttpHeader>::iterator HttpHeaders::end( ) {
					return headers.end( );
				}

				std::vector<HttpHeader>::const_iterator HttpHeaders::cbegin( ) const {
					return headers.cbegin( );
				}

				std::vector<HttpHeader>::const_iterator HttpHeaders::cend( ) const {
					return headers.cend( );
				}

				HttpHeaders::HttpHeaders( ) : headers( ) { }
				HttpHeaders::HttpHeaders( std::initializer_list<HttpHeader> values ) : headers( std::begin( values ), std::end( values ) ) { }

				std::vector<HttpHeader>::iterator HttpHeaders::find( boost::string_ref header_name ) {
					auto it = std::find_if( std::begin( headers ), std::end( headers ), [&header_name]( HttpHeader const & item ) {
						return 0 == header_name.compare( item.name );
					} );
					return it;
				}

				std::vector<HttpHeader>::const_iterator HttpHeaders::find( boost::string_ref header_name ) const {
					auto it = std::find_if( std::begin( headers ), std::end( headers ), [&header_name]( HttpHeader const & item ) {
						return 0 == header_name.compare( item.name );
					} );
					return it;
				}

				std::string const & HttpHeaders::operator[]( boost::string_ref header_name ) const {
					return find( header_name )->value;
				}

				std::string & HttpHeaders::operator[]( boost::string_ref header_name ) {
					auto it = find( header_name );
					if( it == headers.end( ) ) {
						it = headers.emplace( headers.end( ), header_name, "" );
					}
					return it->value;
				}

				bool HttpHeaders::exits( boost::string_ref header_name ) const {
					return find( header_name ) != headers.cend( );
				}

				std::string const & HttpHeaders::at( boost::string_ref header_name ) const {
					auto it = HttpHeaders::find( header_name );
					if( it != std::end( headers ) ) {
						return it->value;
					}
					throw std::out_of_range( header_name.to_string( ) + " is not a valid header" );
				}

				std::string & HttpHeaders::at( boost::string_ref header_name ) {
					auto it = HttpHeaders::find( header_name );
					if( it != std::end( headers ) ) {
						return it->value;
					}
					throw std::out_of_range( header_name.to_string( ) + " is not a valid header" );
				}

				std::string HttpHeaders::to_string( ) {
					std::stringstream ss;
					for( auto const & header : headers ) {
						ss <<header.to_string( ) <<"\r\n";
					}
					return ss.str( );
				}

				HttpHeaders& HttpHeaders::add( std::string header_name, std::string header_value ) {
					headers.emplace_back( std::move( header_name ), std::move( header_value ) );
					return *this;
				}
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw

