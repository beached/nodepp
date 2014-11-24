

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

#include "lib_http.h"

#include <iostream>

void f( int a ) {
	std::cout << 2 * a << std::endl;
}



int main( int, char const ** ) {
	using namespace daw::nodepp::lib;

	auto server = http::create_server( (std::function<void( http::ClientRequest req, http::ServerResponse resp )>)[]( http::ClientRequest req, http::ServerResponse resp ) {

	} ).listen( 8080 );


	system( "pause" );
	return EXIT_SUCCESS;
}
