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

#include "lib_http_client_connection_options.h"
#include <boost/utility/string_ref.hpp>

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				HttpClientConnectionOption::HttpClientConnectionOption( std::string key, HttpClientConnectionOption::value_type value ): m_value( std::make_pair<std::string, HttpClientConnectionOption::value_type>( std::move( key ), std::move( value ) ) ) { }
//				HttpClientConnectionOption::HttpClientConnectionOption( std::pair<std::string, HttpClientConnectionOption::value_type> key_value ): m_value( std::move( key_value ) ) { }

				HttpClientConnectionOption & HttpClientConnectionOption::operator=( std::pair<std::string, HttpClientConnectionOption::value_type> key_value ) {
					m_value = std::move( key_value );
					return *this;
				}

				HttpClientConnectionOption::operator std::pair<std::string, HttpClientConnectionOption::value_type>( ) const {
					return m_value;
				}

				HttpClientConnectionOptions::HttpClientConnectionOptions( std::initializer_list<std::pair<std::string const, HttpClientConnectionOptions::value_type>> values ): m_dictionary( std::move( values ) ) { }

				HttpClientConnectionOptions & HttpClientConnectionOptions::operator=( std::initializer_list<std::pair<std::string const, value_type>> values ) {
					m_dictionary = dictionary_t { std::move( values ) };
					return *this;
				}


				HttpClientConnectionOptions::HttpClientConnectionOptions( std::initializer_list<HttpClientConnectionOption> values ) { }

				HttpClientConnectionOptions & HttpClientConnectionOptions::operator=( std::initializer_list<HttpClientConnectionOption> values ) {

					return *this;
				}

				size_t HttpClientConnectionOptions::size( ) const {
					return m_dictionary.size( );
				}

				void HttpClientConnectionOptions::clear( ) {
					m_dictionary.clear( );
				}

				std::vector<std::string> HttpClientConnectionOptions::keys( ) const {
					std::vector<std::string> keys;
					keys.reserve( m_dictionary.size( ) );
					for( auto const & kv : m_dictionary ) {
						keys.push_back( kv.first );
					}
					return keys;
				}

				void HttpClientConnectionOptions::erase( boost::string_ref key ) {
					m_dictionary.erase( key.to_string( ) );
				}
			}	// namespace http
		} // namespace lib
	}	// namespace nodepp
}	// namespace daw

