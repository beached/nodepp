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

#include <boost/asio/buffer.hpp>
#include <boost/utility/string_ref.hpp>
#include <memory>

#include "base_types.h"

namespace daw {
	namespace nodepp {
		namespace base {
			using MutableBuffer = boost::asio::mutable_buffers_1;
			struct write_buffer final {
				using data_type = base::data_t::pointer;
				std::shared_ptr<base::data_t> buff;

				template<typename Iterator>
				write_buffer( Iterator first, Iterator last ): buff( std::make_shared<base::data_t>( first, last ) ) { }

				explicit write_buffer( base::data_t const & source );
				explicit write_buffer( boost::string_ref source );
				~write_buffer( ) = default;
				write_buffer( write_buffer const & ) = default;
				write_buffer( write_buffer && ) = default;
				write_buffer & operator=( write_buffer const & ) = default;
				write_buffer & operator=( write_buffer && ) = default;

				std::size_t size( ) const;

				data_type data( ) const;
				MutableBuffer asio_buff( ) const;
			};	// struct write_buffer
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw

