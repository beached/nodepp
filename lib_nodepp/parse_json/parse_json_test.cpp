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

#define BOOST_TEST_MODULE parse_json_test
#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "daw_json.h"
#include "daw_json_link.h"

using namespace daw::json;

struct Streamable {
	std::string a;
	Streamable( ) : a( "This is a test!" ) { }

	bool operator==(Streamable const & rhs) const {
		return a == rhs.a;
	}

	bool operator!=(Streamable const & rhs) const {
		return a != rhs.a;
	}
};

std::istream& operator>>(std::istream& is, Streamable & value) {
	auto const data_size = static_cast<size_t>(is.rdbuf( )->in_avail( ));
	value.a.reserve( data_size );
	is.read( &value.a[0], data_size );
	return is;
}

std::ostream& operator<<(std::ostream& os, Streamable const & value) {
	os << value.a;
	return os;
}

struct A: public daw::json::JsonLink<A> {
	int a;
	int b;
	double c;
	std::vector<int> d;
	bool e;
	A( ) : JsonLink<A>( ), a( 1 ), b( 2 ), c( 1.23456789 ), d( 100, 5 ), e( true ) {
		link_integral( "a", a );
		link_integral( "b", b );
		link_real( "c", c );
		link_array( "d", d );
		link_boolean( "e", e );
	}

	bool operator==(A const & rhs) const {
		return a == rhs.a && b == rhs.b && c == rhs.c && std::equal( d.begin( ), d.end( ), rhs.d.begin( ) );
	}

	bool operator!=(A const & rhs) const {
		return a != rhs.a || b != rhs.b || c != rhs.c || !std::equal( d.begin( ), d.end( ), rhs.d.begin( ) );
	}
};

struct B: public daw::json::JsonLink<B> {
	A a;
	std::string b;
	Streamable c;
	double d;
	B( ) : JsonLink<B>( ), a( ), b( "hello" ), c( ), d( 1.9233434e-12 ) {
		link_object( "a", a );
		link_string( "b", b );
		link_streamable( "c", c );
		link_real( "d", d );
	}

	bool operator==(B const & rhs) const {
		return a == rhs.a && b == rhs.b && c == rhs.c;
	}

	bool operator!=(B const & rhs) const {
		return a != rhs.a || b != rhs.b || c != rhs.c;
	}
};

template<typename K, typename V>
bool operator==( std::unordered_map<K, V> const & a, std::unordered_map<K, V> const & b ) {
	return a.size( ) == a.size( ) && std::equal( a.begin( ), a.end( ), b.begin( ) );
}

template<typename Stream>
auto fsize( Stream & stream ) -> decltype(stream.tellg( )) {
	auto cur_pos = stream.tellg( );
	stream.seekg( 0, std::fstream::end );
	auto result = stream.tellg( );
	stream.seekg( cur_pos );
	return result;
}

BOOST_AUTO_TEST_CASE( SimpleTest ) {
	B b;
	auto enc = b.encode( );
	auto parsed = daw::json::parse_json( enc );
	B c;
	c.decode( parsed );
	BOOST_CHECK_EQUAL( b, c );
}

BOOST_AUTO_TEST_CASE( MapValues ) {
	std::unordered_map<std::string, B> test_umap;
	test_umap["a"] = B( );
	auto enc = daw::json::generate::value_to_json( "test_umap", test_umap );
	auto parsed = daw::json::parse_json( enc );
	std::unordered_map<std::string, B> test_umap2;
	daw::json::parse::json_to_value( test_umap2, *parsed );
	BOOST_REQUIRE( test_umap == test_umap2 );
}
