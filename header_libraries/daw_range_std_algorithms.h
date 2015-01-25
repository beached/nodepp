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

#include <algorithm>

#include "daw_traits.h"

template<typename Container, typename UnaryPredicate, typename std::enable_if<daw::traits::is_container_like<Container>::value, long> = 0>
bool all_of( Container const & container, UnaryPredicate predicate ) {
	return std::all_of( std::begin( container ), std::end( container ), std::move( predicate ) );
}

template<typename Container, typename UnaryPredicate, typename std::enable_if<daw::traits::is_container_like<Container>::value, long> = 0>
bool any_of( Container const & container, UnaryPredicate predicate ) {
	return std::any_of( std::begin( container ), std::end( container ), std::move( predicate ) );
}

template<typename Container, typename UnaryPredicate, typename std::enable_if<daw::traits::is_container_like<Container>::value, long> = 0>
bool none_of( Container const & container, UnaryPredicate predicate ) {
	return std::none_of( std::begin( container ), std::end( container ), std::move( predicate ) );
}

template<typename Container, typename T, typename std::enable_if<daw::traits::is_container_like<Container>::value, long> = 0>
auto find( Container const & container, T const & value ) -> decltype(std::begin( container )) {
	return std::find( std::begin( container ), std::end( container ), value );
}

template<typename Container, typename UnaryPredicate, typename std::enable_if<daw::traits::is_container_like<Container>::value, long> = 0>
auto find_if( Container const & container, UnaryPredicate predicate ) -> decltype(std::begin( container )) {
	return std::find_if( std::begin( container ), std::end( container ), std::move( predicate ) );
}

template<typename Container, typename UnaryPredicate, typename std::enable_if<daw::traits::is_container_like<Container>::value, long> = 0>
auto find_if_not( Container const & container, UnaryPredicate predicate ) -> decltype(std::begin( container )) {
	return std::find_if_not( std::begin( container ), std::end( container ), std::move( predicate ) );
}
