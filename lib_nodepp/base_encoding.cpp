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

#include <stdexcept>
#include <string>
#include <vector>
#include "base_enoding.h"
#include <boost/utility/string_ref.hpp>

namespace daw {
	namespace nodepp {
		namespace base {
			std::vector<std::string> const & Encoding::valid_enodings( ) {
				static const std::vector<std::string> result = { "ascii", "utf8", "utf16le", "ucs2", "hex" };
				return result;
			}

			Encoding::Encoding( ): m_encoding( "utf8" ) { }

			Encoding::Encoding( std::string encoding ) : m_encoding( encoding ) { }

			Encoding& Encoding::operator = ( boost::string_ref rhs ) {
				if( !is_valid_encoding( rhs ) ) {
					throw std::runtime_error( "Encoding is not valid" );
				}
				m_encoding = rhs.to_string( );
				return *this;
			}

			boost::string_ref Encoding::operator()( ) const { return m_encoding; }

			void Encoding::set( std::string encoding ) {
				if( !is_valid_encoding( encoding ) ) {
					throw std::runtime_error( "Encoding is not valid" );
				}
				m_encoding = encoding;
			}

			bool Encoding::is_valid_encoding( boost::string_ref ) {
				// TODO: validate the encoding
				return true;
			}
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw

