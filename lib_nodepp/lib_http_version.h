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
#include <cstdint>
#include <string>
#include <utility>

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				class HttpVersion final {
					std::pair<uint_fast8_t, uint_fast8_t> m_version;
					bool m_is_valid;
				public:
					HttpVersion( );
					~HttpVersion( ) = default;

					HttpVersion( HttpVersion const & ) = default;
					HttpVersion( HttpVersion && ) = default;
					HttpVersion& operator=( HttpVersion const & ) = default;
					HttpVersion& operator=( HttpVersion && ) = default;
					HttpVersion& operator=( boost::string_ref version );

					explicit HttpVersion( boost::string_ref version );
					HttpVersion( uint_fast8_t Major, uint_fast8_t Minor );							

					uint_fast8_t const & major( ) const;
					uint_fast8_t & major( );
					uint_fast8_t const & minor( ) const;
					uint_fast8_t & minor( );

					std::string to_string( ) const;
					operator std::string( ) const;
					bool is_valid( ) const;
				};
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw

