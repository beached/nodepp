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
#include <iostream>
#include <boost/utility/string_ref.hpp>
#include "../third_party/include/utf8/unchecked.h"

namespace daw {
	namespace range {
			using CharIterator = char const *;
			using UTFIterator = utf8::unchecked::iterator<CharIterator>;
			using UTFValType = UTFIterator::value_type;

			size_t hash_sequence( CharIterator first, CharIterator const last );

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
				CharRange( char const * str );
				iterator begin( );
				const_iterator begin( ) const;
				iterator end( );
				const_iterator end( ) const;
				size_t size( ) const;
				bool empty( ) const;
				CharRange & operator++( );
				CharRange operator++( int );
				void advance( size_t const n );
				void safe_advance( size_t const count );
				void set( iterator Begin, iterator End, difference_type Size = -1 );
				CharRange & operator+=( size_t const n );
			};	// struct CharRange

			CharRange operator+( CharRange range, size_t const n );


			CharRange create_char_range( UTFIterator const first,  UTFIterator const last );
			CharRange create_char_range( boost::string_ref const & str );
			CharRange create_char_range( CharIterator first, CharIterator last );

			bool operator==( CharRange const & first, CharRange const & second );
			bool operator==( CharRange const & first, boost::string_ref const & second );

			void clear( CharRange & str );
			std::string to_string( CharRange const & str );

			std::ostream& operator<<( std::ostream & os, CharRange const & value );
			
			boost::string_ref to_string_ref( CharRange const & str );


			bool at_end( CharRange const & range );
	}	// namespace range
}	// namespace daw

namespace std {
	template<>
	struct hash<daw::range::CharRange> {
		size_t operator()( daw::range::CharRange const & value ) const {
			return daw::range::hash_sequence( value.begin( ).base( ), value.end( ).base( ) );
		}
	};
}
