

// #include "lib_http.h"
// 
// using namespace daw::nodepp::lib;

#include <boost/any.hpp>
#include <cstdint>
#include <functional>
#include <iostream>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "base_event_emitter.h"
#include "lib_http.h"
#include "lib_http_server.h"
#include "range_algorithm.h"
#include "range_algorithm.h"
#include "utility.h"
#include "utility.h"


int main( int, char const ** ) {
	using namespace daw::nodepp;
	using listen_t = std::function < void( lib::http::HttpClientRequest, lib::http::HttpServerResponse ) > ;
	auto server = lib::http::create_server( []( lib::http::HttpClientRequest request, lib::http::HttpServerResponse response ) {
		response.write_head( 200, "", { { "Content-Type", "text/plain" } } );
		response.write( "Hello World" );
		response.end( );
	} ).listen( 8080 );

	while( true ) { }
	system( "pause" );
	return EXIT_SUCCESS;
}
