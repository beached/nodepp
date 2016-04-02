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

#include "base_write_buffer.h"

namespace daw {
	namespace nodepp {
		namespace base {
			write_buffer::write_buffer( base::data_t const & source ): buff( std::make_shared<base::data_t>( source ) ) { }

			write_buffer::write_buffer( boost::string_ref source ) : buff( std::make_shared<base::data_t>( source.begin( ), source.end( ) ) ) { }

			std::size_t write_buffer::size( ) const {
				return buff->size( );
			}

			write_buffer::data_type write_buffer::data( ) const {
				return buff->data( );
			}

			MutableBuffer write_buffer::asio_buff( ) const {
				return boost::asio::buffer( data( ), size( ) );
			}
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw

