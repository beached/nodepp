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

namespace daw {
	namespace mixins {
		//////////////////////////////////////////////////////////////////////////
		/// Summary:	Provides a minimal interface for a vector like container
		///				with a member that is a container.
		///				Requires a member in Derived called container( ) that
		///				returns the container
		template<typename Derived>
		class VectorLike {
		private:
			Derived& self( ) {
				return *dynamic_cast<Derived*>(this);
			}

			Derived const & self( ) const {
				return *dynamic_cast<Derived const *>(this);
			}
			using container_t = decltype(std::declval<Derived>( ).container( ));

		public:
			using iterator = typename container_t::iterator;
			using const_iterator = typename container_t::const_iterator;
			using value_type = typename container_t::value_type;
			using reference = typename container_t::reference;
			using const_reference = typename container_t::const_reference;
			using size_type = typename container_t::size_type;

			auto begin( ) -> decltype(self( ).container( ).begin( )) {
				return self( ).container( ).begin( );
			}

			auto end( ) -> decltype(self( ).container( ).end( )) {
				return self( ).container( ).end( );
			}

			auto cbegin( ) -> decltype(self( ).container( ).cbegin( )) {
				return self( ).container( ).cbegin( );
			}

			auto cend( ) -> decltype(self( ).container( ).cend( )) {
				return self( ).container( ).cend( );
			}

			reference operator[]( size_type n ) {
				return self( ).container( )[n];
			}

			const_reference operator[]( size_type n ) const {
				return self( ).container( )[n];
			}

			reference at( size_type n ) {
				return self( ).container( ).at( n );
			}

			const_reference at( size_type n ) const {
				return self( ).container( ).at( n );
			}

			void push_back( value_type && value ) {
				self( ).container( ).push_back( std::move( value ) );
			}

			void push_back( const_reference value ) {
				self( ).container( ).push_back( value );
			}

			template<typename... Args>
			void emplace_back( Args&&... args ) {
				self( ).container( ).emplace_back( std::forward<Args>( args )... );
			}
		};
	}	// namespace mixins
}	// namespace daw
