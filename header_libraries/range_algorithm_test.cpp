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

#include <boost/iterator/counting_iterator.hpp>
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "range_algorithm.h"

BOOST_AUTO_TEST_CASE( range_algorithm_none_yet ) {
	BOOST_WARN_MESSAGE( true, "range_algorithm_test unimplemented" );
}

BOOST_AUTO_TEST_CASE( range_algorithm_accumulate ) {
	std::vector<int> test( 100, 1 );
	auto sum = daw::algorithm::accumulate( test, 0 );
	BOOST_REQUIRE_MESSAGE( static_cast<int>( test.size( ) ) == sum, "A vector of 1's size should equal it's sum" );

	auto product = daw::algorithm::accumulate( test, 1, []( int const & lhs, int const & rhs ) {
		return lhs * rhs;
	} );

	BOOST_REQUIRE_MESSAGE( 1 == product, "The product of a vector of 1's should be 1" );
}

BOOST_AUTO_TEST_CASE( range_algorithm_map ) {
	std::vector<int> test_vec( boost::counting_iterator<int>( 1 ), boost::counting_iterator<int>( 100 ) );

	auto result = daw::algorithm::map( test_vec, []( int const & val ) {
		return 2 * val;
	} );

	BOOST_REQUIRE_MESSAGE( test_vec.size( ) == result.size( ), "Result of map should be of equal size to the input" );

	auto sum1 = daw::algorithm::accumulate( test_vec, 0 );
	auto sum2 = daw::algorithm::accumulate( result, 0 );

	BOOST_REQUIRE_MESSAGE( sum2 == 2 * sum1, "The result should have double the sum of the test_vec" );
}

