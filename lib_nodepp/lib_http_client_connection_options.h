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
#include <unordered_map>
#include <boost/variant.hpp>
#include <boost/utility/string_ref.hpp>
#include <boost/optional.hpp>
#include <vector>

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				struct HttpClientConnectionOption final {
					using value_type = boost::variant<bool, int64_t, double, std::string, std::initializer_list<bool>, std::initializer_list<int64_t>, std::initializer_list<double>, std::initializer_list<std::string>>;
				private:
					std::pair<std::string, value_type> m_value;
				public:
					HttpClientConnectionOption( ) = default;
					HttpClientConnectionOption( HttpClientConnectionOption const & ) = default;
					HttpClientConnectionOption( HttpClientConnectionOption && ) = default;
					HttpClientConnectionOption & operator=( HttpClientConnectionOption const & ) = default;
					HttpClientConnectionOption & operator=( HttpClientConnectionOption && ) = default;
					~HttpClientConnectionOption( ) = default;

					HttpClientConnectionOption( std::string key, value_type value );

					HttpClientConnectionOption & operator=( std::pair<std::string, value_type> key_value );

					operator std::pair<std::string, value_type>( ) const;
				};

				struct HttpClientConnectionOptions final {
					//typedef boost::variant<bool, int64_t, double, std::string> value_type;
					using value_type = boost::variant<bool, int64_t, double, std::string, std::initializer_list<bool>, std::initializer_list<int64_t>, std::initializer_list<double>, std::initializer_list<std::string>>;
				private:
					using dictionary_t = std::unordered_map <std::string, value_type>;
					dictionary_t m_dictionary;
				public:
					HttpClientConnectionOptions( ) = default;
					HttpClientConnectionOptions( HttpClientConnectionOptions const & ) = default;
					HttpClientConnectionOptions( HttpClientConnectionOptions && ) = default;
					HttpClientConnectionOptions & operator=( HttpClientConnectionOptions const & ) = default;
					HttpClientConnectionOptions & operator=( HttpClientConnectionOptions && ) = default;
					~HttpClientConnectionOptions( ) = default;

					HttpClientConnectionOptions( std::initializer_list<std::pair<std::string const, value_type>> values );
					HttpClientConnectionOptions & operator=( std::initializer_list<std::pair<std::string const, value_type>> values );

					HttpClientConnectionOptions( std::initializer_list<HttpClientConnectionOption> values );
					HttpClientConnectionOptions & operator=( std::initializer_list<HttpClientConnectionOption> values );

					size_t size( ) const;
					void clear( );
					std::vector<std::string> keys( ) const;
					void erase( boost::string_ref key );

					template<typename T>
					T get( boost::string_ref key ) const {
						return boost::get<T>( m_dictionary.at( key.to_string( ) ) );
					}

				};
			}	// namespace http
		} // namespace lib
	}	// namespace nodepp
}	// namespace daw

