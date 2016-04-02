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
#include <boost/utility/string_ref.hpp>
#include <string>
#include <vector>

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				struct HttpHeader final {
					std::string name;
					std::string value;

					HttpHeader( );
					HttpHeader( std::string Name, std::string Value );
					HttpHeader( boost::string_ref Name, boost::string_ref Value );
					~HttpHeader( ) = default;
					HttpHeader( HttpHeader const & ) = default;
					HttpHeader( HttpHeader && ) = default;
					HttpHeader & operator=( HttpHeader const & ) = default;
					HttpHeader & operator=( HttpHeader && ) = default;

					std::string to_string( ) const;
					bool empty( ) const;
				};

				struct HttpHeaders final {
					std::vector<HttpHeader> headers;

					HttpHeaders( );
					HttpHeaders( std::initializer_list<HttpHeader> values );
					~HttpHeaders( ) = default;
					HttpHeaders( HttpHeaders const & ) = default;
					HttpHeaders( HttpHeaders && ) = default;
					HttpHeaders& operator=( HttpHeaders const & ) = default;
					HttpHeaders& operator=( HttpHeaders && ) = default;

					std::vector<HttpHeader>::iterator begin( );
					std::vector<HttpHeader>::iterator end( );
					std::vector<HttpHeader>::const_iterator cbegin( ) const;
					std::vector<HttpHeader>::const_iterator cend( ) const;
					std::vector<HttpHeader>::iterator find( boost::string_ref header_name );
					std::vector<HttpHeader>::const_iterator find( boost::string_ref header_name ) const;

					std::string const & operator[]( boost::string_ref header_name ) const;
					std::string & operator[]( boost::string_ref header_name );

					std::string const & at( boost::string_ref header_name ) const;
					std::string & at( boost::string_ref header_name );

					bool exits( boost::string_ref header_name ) const;
					std::string to_string( );

					HttpHeaders& add( std::string header_name, std::string header_value );
				};
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw

