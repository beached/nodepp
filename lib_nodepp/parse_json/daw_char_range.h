// The MIT License (MIT)
//
// Copyright (c) 2014-2015 Darrell Wright
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
#include "../third_party/include/utf8/unchecked.h"

namespace daw {
	namespace nodepp {
		namespace base {
			using CharIterator = char const *;
			using UTFIterator = utf8::unchecked::iterator<CharIterator>;
			using UTFValType = UTFIterator::value_type;

			size_t hash_sequence( nodepp::base::CharIterator first, CharIterator const last );

			struct CharRange {
				using iterator = UTFIterator;
				using const_iterator = UTFIterator const;
				using reference = UTFIterator::reference;
				using value_type = UTFIterator::value_type;
				using const_reference = value_type const &;
				using difference_type = UTFIterator::difference_type;
			private:
				iterator m_begin;
				iterator m_end;
				size_t m_size;
			public:
				CharRange( ) = delete;
				~CharRange( ) = default;
				CharRange( CharRange const & ) = default;
				CharRange & operator=( CharRange const & ) = default;
				CharRange( CharRange && ) = default;
				CharRange & operator=( CharRange && ) = default;
				CharRange( iterator Begin, iterator End );
				iterator begin( );
				const_iterator begin( ) const;
				iterator end( );
				const_iterator end( ) const;
				size_t size( ) const;
				CharRange & operator++( );
				CharRange operator++( int );
				void advance( size_t const n );
				void safe_advance( size_t const count );
				void set( iterator Begin, iterator End, difference_type Size = -1 );
			};	// struct CharRange

			bool at_end( CharRange const & range );
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
