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
#include "lib_net_address.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				NetAddress::NetAddress( ) : m_address( "0.0.0.0" ) { }

				NetAddress::NetAddress( std::string address ) : m_address( std::move( address ) ) {
					if( !is_valid( m_address ) ) {
						throw std::runtime_error( "Invalid address" );
					}
				}

				boost::string_ref NetAddress::operator()( ) const {
					return m_address;
				}

				bool NetAddress::is_valid( std::string address ) {
					return true;	// TODO: complete
				}
			}	// namespace lib
		}	// namespace net
	}	// namespace nodepp
}	// namespace daw

