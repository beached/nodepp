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
		template<typename Derived, typename container_type>
		class VectorLike {
		private:
			Derived & derived( ) {
				return *static_cast<Derived*>(this);
			}

			Derived const & derived( ) const {
				return *static_cast<Derived const*>(this);
			}
		public:
			using conainter_type = container_type;
			using iterator = typename container_type::iterator;
			using const_iterator = typename container_type::const_iterator;
			using value_type = typename container_type::value_type;
			using reference = typename container_type::reference;
			using const_reference = typename container_type::const_reference;
			using size_type = typename container_type::size_type;

			iterator begin( ) {
				return derived( ).container( ).begin( );
			}

			iterator end( ) {
				return derived( ).container( ).end( );
			}

			const_iterator begin( ) const {
				return derived( ).container( ).begin( );
			}

			const_iterator end( ) const {
				return derived( ).container( ).end( );
			}

			const_iterator cbegin( ) {
				return derived( ).container( ).cbegin( );
			}

			const_iterator cend( ) {
				return derived( ).container( ).cend( );
			}

			void push_back( value_type && value ) {
				derived( ).container( ).insert( end( ), value );
			}

			void push_back( const_reference value ) {
				derived( ).container( ).insert( end( ), value );
			}

			template<typename... Args>
			void emplace_back( Args&&... args ) {
				derived( ).container( ).emplace( end( ), std::forward<Args>( args )... );
			}

			iterator insert( iterator where, value_type item ) {
				return derived( ).container( ).insert( where, std::move( item ) );
			}

			reference operator[]( size_type pos ) {
				return *(begin( ) + pos);
			}

			const_reference operator[]( size_type pos ) const {
				return *(cbegin( ) + pos);
			}

			size_type size( ) const {
				return std::distance( cbegin( ), cend( ) );
			}
		};

		/////////////////////////////////////11/////////////////////////////////////
		/// Summary:	Provides a minimal interface for a map like container
		///				with a member that is a container.
		///				Requires a member in Derived called container( ) that
		///				returns the container, find( ) that searches
		///				for a key and key_type and mapped_type
		template<typename Derived, typename MapType>
		class MapLike {
		private:
			Derived & derived( ) {
				return *static_cast<Derived*>(this);
			}

			Derived const & derived( ) const {
				return *static_cast<Derived const*>(this);
			}
		public:
			using container_type = MapType;
			using iterator = typename container_type::iterator;
			using const_iterator = typename container_type::const_iterator;
			using value_type = typename container_type::value_type;
			using reference = typename container_type::reference;
			using const_reference = typename container_type::const_reference;
			using size_type = typename container_type::size_type;

			iterator begin( ) {
				return derived( ).container( ).begin( );
			}

			iterator end( ) {
				return derived( ).container( ).end( );
			}

			const_iterator begin( ) const {
				return derived( ).container( ).begin( );
			}

			const_iterator end( ) const {
				return derived( ).container( ).end( );
			}

			const_iterator cbegin( ) {
				return derived( ).container( ).cbegin( );
			}

			const_iterator cend( ) {
				return derived( ).container( ).cend( );
			}

			void push_back( value_type && value ) {
				derived( ).container( ).insert( end( ), value );
			}

			void push_back( const_reference value ) {
				derived( ).container( ).insert( end( ), value );
			}

			template<typename... Args>
			void emplace_back( Args&&... args ) {
				derived( ).container( ).emplace( end( ), std::forward<Args>( args )... );
			}

			iterator insert( iterator where, value_type item ) {
				return derived( ).container( ).insert( where, std::move( item ) );
			}

			reference operator[]( typename container_type::key_type key ) {
				return derived( ).find( key );
			}

			const_reference operator[]( typename container_type::key_type key ) const {
				return derived( ).find( key );
			}

			size_type size( ) const {
				return std::distance( cbegin( ), cend( ) );
			}
		};
	}	// namespace mixins
}	// namespace daw
