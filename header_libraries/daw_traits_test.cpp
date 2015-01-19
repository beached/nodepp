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

#define BOOST_TEST_MODULE daw_traits_test
#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "daw_traits.h"

BOOST_AUTO_TEST_CASE( is_equality_comparable_test ) {
	BOOST_REQUIRE_MESSAGE( true == daw::traits::is_equality_comparable<std::string>::value, "1. std::string should report as being equality comparable" );
	BOOST_REQUIRE_MESSAGE( true == daw::traits::is_equality_comparable<std::vector<std::string>>::value, "2. std::vector should report as being equality comparable" );
	{
		struct NotEqual {
			int x;
		private:
		};
		BOOST_REQUIRE_MESSAGE( false == daw::traits::is_equality_comparable<NotEqual>::value, "3. NotEqual struct should not report as being equality comparable" );
	}
	{
		class NotEqual2 {
		private:
			int x;
			std::vector<int> y;
		public:
			NotEqual2( int, std::vector<int> ) { }
		};
		BOOST_REQUIRE_MESSAGE( false == daw::traits::is_equality_comparable<NotEqual2>::value, "4. NotEqual2 struct should not report as being equality comparable" );
	}
}

BOOST_AUTO_TEST_CASE( is_regular_test ) {
	BOOST_REQUIRE_MESSAGE( true == daw::traits::is_regular<std::string>::value, "1. std::string should report as being regular" );
	struct NotRegular {
		int x;
		NotRegular( int );
		NotRegular( ) = delete;
		NotRegular( NotRegular const & ) = delete;
	};
	BOOST_REQUIRE_MESSAGE( false == daw::traits::is_regular<NotRegular>::value, "2. struct NotRegular should report as being regular" );
}

BOOST_AUTO_TEST_CASE( max_sizeof_test ) {
	auto result = daw::traits::max_sizeof<int8_t, int16_t, uint8_t, int, int32_t, int64_t, uint64_t, size_t>::value;
	BOOST_REQUIRE_MESSAGE( result == sizeof( size_t ), "1. Max sizeof did not report the size of the largest item" );
}

BOOST_AUTO_TEST_CASE( are_true_test ) {
	{
		auto result = daw::traits::are_true( true, true, 1, true );
		BOOST_REQUIRE_MESSAGE( true == result, "1. are_true should have reported true" );
	}
	{
		auto result = daw::traits::are_true( true, true, false, true, 0 );
		BOOST_REQUIRE_MESSAGE( false == result, "2. are_true should have reported false" );
	}

	{
		auto result = daw::traits::are_true( true, 0 );
		BOOST_REQUIRE_MESSAGE( false == result, "3. are_true should have reported false" );
	}
}

BOOST_AUTO_TEST_CASE( are_same_types_test ) {
	{
		auto result = daw::traits::are_same_types<bool, std::string, std::vector<int>>::value;
		BOOST_REQUIRE_MESSAGE( false == result, "1. Different types reported as same" );
	}
	{
		auto result = daw::traits::are_same_types<std::string, std::string, std::string>::value;
		BOOST_REQUIRE_MESSAGE( true == result, "2. Same types reported as different" );
	}
}

//////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( has_begin_member_test ) {
	BOOST_REQUIRE_MESSAGE( true == daw::traits::has_begin_member<std::string>::value, "std::string should have a begin( ) method" );
	BOOST_REQUIRE_MESSAGE( true == daw::traits::has_begin_member<std::vector<int>>::value, "std::vector should have a begin( ) method" );
	{
		using test_t = std::unordered_map < std::string, int > ;// Macro's and comma parameters
		BOOST_REQUIRE_MESSAGE( true == daw::traits::has_begin_member<test_t>::value, "std::unordered should have a begin( ) method" );
	}
	{
		struct T {
			int x;
		};
		BOOST_REQUIRE_MESSAGE( false == daw::traits::has_begin_member<T>::value, "struct T should not have a begin( ) method" );
	}
}

BOOST_AUTO_TEST_CASE( has_end_member_test ) {
	BOOST_REQUIRE_MESSAGE( true == daw::traits::has_end_member<std::string>::value, "std::string should have a end( ) method" );
	BOOST_REQUIRE_MESSAGE( true == daw::traits::has_end_member<std::vector<int>>::value, "std::vector should have a end( ) method" );
	{
		using test_t = std::unordered_map < std::string, int > ;// Macro's and comma parameters
		BOOST_REQUIRE_MESSAGE( true == daw::traits::has_end_member<test_t>::value, "std::unordered should have a end( ) method" );
	}
	{
		struct T {
			int x;
		};
		BOOST_REQUIRE_MESSAGE( false == daw::traits::has_end_member<T>::value, "struct T should not have a end( ) method" );
	}
}
