

// #include "lib_http.h"
// 
// using namespace daw::nodepp::lib;

#include <boost/any.hpp>
#include <cstdint>
#include <functional>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include "range_algorithm.h"
#include "utility.h"

#include "lib_event_emitter.h"

#include <iostream>

void f( int a ) {
	std::cout << 2 * a << std::endl;
}



int main( int, char const ** ) {
	enum class blah { a = 0, b, c };
	daw::nodepp::base::EventEmitter test;
	auto g = []( int a ) {
		std::cout << a << std::endl;
	};

	std::function<void( int )> h = g;

	test.add_listener( "b", h );
	std::function<void( int )> i = f;
	test.add_listener( "b", f, true );
	test.add_listener( "b", std::function<void( int )>( []( int x ) { std::cout << x * 3 << std::endl; } ) );
	test.emit( "b", 500 );
	
	test.emit( "b", 100 );

	// 	auto server = http::create_server( []( http::Request req, http::Response resp ) {
	// 
	// 	} ).listen( 8080 );


	system( "pause" );
	return EXIT_SUCCESS;
}
