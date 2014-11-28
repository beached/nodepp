

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
#include "base_event_emitter.h"
#include "lib_http.h"
#include "lib_http_server.h"

#include <iostream>

void f( int a ) {
	std::cout << 2 * a << std::endl;
}

#include "range_algorithm.h"
#include "utility.h"



int main( int, char const ** ) {
// 	using namespace daw::nodepp;
// 	using listen_t = std::function < void( lib::http::HttpClientRequest, lib::http::HttpServerResponse ) > ;
// 	auto server = lib::http::create_server( static_cast<listen_t>( []( lib::http::HttpClientRequest request, lib::http::HttpServerResponse response ) {
// 		response.write_head( 200, "", { { "Content-Type", "text/plain" } } );
// 		response.write( "Hello World" );
// 		response.end( );
// 	}) ).listen( 8080 );
// 
// 	while( true ) { }

	auto b = std::function<void( int )>([]( int b ) { 
		std::cout << "std::function " << b << std::endl;
	} );

	daw::nodepp::base::EventEmitter a;
	a.on( "a", f );
	a.on( "a", b );
	a.on( "a", std::function<void( int )>( []( int b ) {
		std::cout << "std::function 2 " << b << std::endl;
	} ) );

	a.emit( "a", 5 );
	system( "pause" );
	return EXIT_SUCCESS;
}
