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

BOOST_AUTO_TEST_CASE( bool_and_test ) {
	{
		auto result = daw::traits::bool_and<true, true, true>::value;
		BOOST_REQUIRE_MESSAGE( true == result, "1. All true's in bool_and should return true" );
	}
	{
		auto result = daw::traits::bool_and<true, false, false>::value;
		BOOST_REQUIRE_MESSAGE( false == result, "2. A mix of true's and false's in bool_and should return false" );
	}
	{
		auto result = daw::traits::bool_and<true>::value;
		BOOST_REQUIRE_MESSAGE( true == result, "3. A true in bool_and should return true" );
	}
	{
		auto result = daw::traits::bool_and<false>::value;
		BOOST_REQUIRE_MESSAGE( false == result, "4. A false in bool_and should return false" );
	}
}

BOOST_AUTO_TEST_CASE( bool_or_test ) {
	{
		auto result = daw::traits::bool_or<true, true, true>::value;
		BOOST_REQUIRE_MESSAGE( true == result, "1. All true's in bool_or should return true" );
	}
	{
		auto result = daw::traits::bool_or<true, false, false>::value;
		BOOST_REQUIRE_MESSAGE( true == result, "2. A mix of true's and false's in bool_or should return true" );
	}
	{
		auto result = daw::traits::bool_or<true>::value;
		BOOST_REQUIRE_MESSAGE( true == result, "3. A true in bool_or should return true" );
	}
	{
		auto result = daw::traits::bool_or<false>::value;
		BOOST_REQUIRE_MESSAGE( false == result, "4. A false in bool_or should return false" );
	}
	{
		auto result = daw::traits::bool_or<false, false>::value;
		BOOST_REQUIRE_MESSAGE( false == result, "5. Two false's in bool_or should return false" );
	}
}

bool enable_if_any_func_test( std::string, ... ) { return false; }

template<typename... Args, typename = typename daw::traits::enable_if_any<bool, std::is_same<bool, Args>::value...>::type>
bool enable_if_any_func_test( Args... args ) {
	return true;
}

BOOST_AUTO_TEST_CASE( enable_if_any_test ) {
	{
		auto result = enable_if_any_func_test( std::string( "" ) );
		BOOST_REQUIRE_MESSAGE( false == result, "1. Enable if any should have defaulted to string only with a std::string value" );
	}
	{
		auto result = enable_if_any_func_test( std::string( ), true, 134, std::string( "dfd" ) );
		BOOST_REQUIRE_MESSAGE( true == result, "2. Enable if any should have ran templated version with multiple params including a boolean" );
	}
	{
		auto result = enable_if_any_func_test( false );
		BOOST_REQUIRE_MESSAGE( true == result, "3. Enable if any should have worked with a false" );
	}
}

bool enable_if_all_func_test( std::string, ... ) { return false; }

template<typename... Args, typename = typename daw::traits::enable_if_all<bool, std::is_same<bool, Args>::value...>::type>
bool enable_if_all_func_test( Args... args ) {
	return true;
}

BOOST_AUTO_TEST_CASE( enable_if_all_test ) {
	{
		auto result = enable_if_all_func_test( std::string( "" ) );
		BOOST_REQUIRE_MESSAGE( false == result, "1. Enable if all should have defaulted to string only with a std::string value" );
	}
	{
		auto result = enable_if_all_func_test( std::string( ), true, 134, std::string( "dfd" ) );
		BOOST_REQUIRE_MESSAGE( false == result, "2. Enable if all should have ran templated version with multiple params including a boolean but failed and defaulted to non-templated version" );
	}
	{
		auto result = enable_if_all_func_test( false );
		BOOST_REQUIRE_MESSAGE( true == result, "3. Enable if any should have worked with a false" );
	}
}

BOOST_AUTO_TEST_CASE( is_one_of_test ) {
	{
		auto result = daw::traits::is_one_of < std::string, std::string, std::string, int, std::vector<std::string>>::value;
		BOOST_REQUIRE_MESSAGE( true == result, "1. Is one of should report true when at least one matches" );
	}
	{
		auto result = daw::traits::is_one_of < std::string, int, int, double, std::vector<std::string>>::value;
		BOOST_REQUIRE_MESSAGE( false == result, "2. Is one of should report false when none matches" );
	}
	{
		auto result = daw::traits::is_one_of < std::string, std::string>::value;
		BOOST_REQUIRE_MESSAGE( true == result, "3. Is one of should report true with a single param and it matches" );
	}
	{
		auto result = daw::traits::is_one_of < std::string, int>::value;
		BOOST_REQUIRE_MESSAGE( false == result, "4. Is one of should report false with a single param and it does not match" );
	}
}

BOOST_AUTO_TEST_CASE( has_begin_member_test ) {
	BOOST_REQUIRE_MESSAGE( true == daw::traits::has_begin_member<std::string>::value, "1. std::string should have a begin( ) method" );
	BOOST_REQUIRE_MESSAGE( true == daw::traits::has_begin_member<std::vector<int>>::value, "2. std::vector should have a begin( ) method" );
	{
		using test_t = std::unordered_map < std::string, int > ;// Macro's and comma parameters
		BOOST_REQUIRE_MESSAGE( true == daw::traits::has_begin_member<test_t>::value, "3. std::unordered should have a begin( ) method" );
	}
	{
		struct T {
			int x;
		};
		BOOST_REQUIRE_MESSAGE( false == daw::traits::has_begin_member<T>::value, "4. struct T should not have a begin( ) method" );
	}
}

BOOST_AUTO_TEST_CASE( has_value_type_member_test ) {
	BOOST_REQUIRE_MESSAGE( true == daw::traits::has_value_type_member<std::string>::value, "1. std::string should have a value_type" );
	struct T {
		int x;
	};
	BOOST_REQUIRE_MESSAGE( false == daw::traits::has_value_type_member<T>::value, "2. T should not have a value_type" );
}

BOOST_AUTO_TEST_CASE( is_container_like_test ) {
	BOOST_REQUIRE_MESSAGE( true == daw::traits::is_container_like<std::string>::value, "1. std::string should be container like" );
	BOOST_REQUIRE_MESSAGE( true == daw::traits::is_container_like<std::vector<std::string>>::value, "2. std::vector<std::string> should be container like" );
	using map_t = std::unordered_map < std::string, int > ;
	BOOST_REQUIRE_MESSAGE( true == daw::traits::is_container_like<map_t>::value, "3. std::unordered_map<std::string, int> should be container like" );
	struct T {
		int x;
	};
	BOOST_REQUIRE_MESSAGE( false == daw::traits::is_container_like<T>::value, "4. T should not be container like" );
}

BOOST_AUTO_TEST_CASE( is_string_test ) {
	BOOST_REQUIRE_MESSAGE( true == daw::traits::is_string<std::string>::value, "1. is_string should return true for std::string" );
	BOOST_REQUIRE_MESSAGE( false == daw::traits::is_string<std::vector<std::string>>::value, "2. is_string should return false for std::vector<std::string>" );
	struct T {
		int x;
	};
	BOOST_REQUIRE_MESSAGE( false == daw::traits::is_string<std::vector<std::string>>::value, "3. is_string should return false for struct T" );
}