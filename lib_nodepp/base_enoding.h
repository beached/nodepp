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
#include <vector>

namespace daw {
	namespace nodepp {
		namespace base {
			//////////////////////////////////////////////////////////////////////////
			// Summary:		Wraps around an string representing the encoding of
			//				binary data (e.g. utf8, binary, hex)
			// Requires:
			class Encoding final {
				std::string m_encoding;
				static std::vector<std::string> const &	valid_enodings( );
			public:
				Encoding( );
				explicit Encoding( std::string encoding );
				Encoding( Encoding const & ) = default;
				Encoding( Encoding && ) = default;
				Encoding & operator=( Encoding const & ) = default;
				Encoding & operator=( Encoding && ) = default;

				Encoding& operator=(boost::string_ref rhs);

				~Encoding( ) = default;

				boost::string_ref operator()( ) const;
				void set( std::string encoding );
				static bool is_valid_encoding( boost::string_ref encoding );

				operator std::string( ) const {
					return m_encoding;
				}
			};	// class Encoding
		}	// namespace base
	}	// namespace nodepp
} 	// namespace daw

