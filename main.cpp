

// #include "lib_http.h"
// 
// using namespace daw::nodepp::lib;

#include <boost/any.hpp>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "base_error.h"
#include "base_event_emitter.h"
#include "base_handle.h"
#include "lib_http.h"
#include "lib_http_server.h"
#include "lib_net_dns.h"
#include "lib_net_socket.h"
#include "range_algorithm.h"
#include "range_algorithm.h"
#include "utility.h"

int main( int, char const ** ) {
	using namespace daw::nodepp;
// 	using listen_t = std::function < void( lib::http::HttpClientRequest, lib::http::HttpServerResponse ) > ;
// 	auto server = lib::http::create_server( []( lib::http::HttpClientRequest request, lib::http::HttpServerResponse response ) {
// 		response.write_head( 200, "", { { "Content-Type", "text/plain" } } );
// 		response.write( "Hello World" );
// 		response.end( );
// 	} ).listen( 8080 );

	auto socket = lib::net::NetSocket( );
	socket.on( "error", []( base::Error const & error ) {
		std::cerr << "Error connecting" << std::endl << error << std::endl;
		exit( EXIT_FAILURE );
	} ).on( "connect", [&]( ) {
		auto const & server = socket.remote_address( );
		auto path = "/";
		std::cout << "Connected to " << socket.remote_address( ) << ":" << socket.remote_port( ) << std::endl;
		std::stringstream request_stream;
		request_stream << "GET " << path << " HTTP/1.0\r\n";
		request_stream << "Host: " << server << "\r\n";
		request_stream << "Accept: */*\r\n";
		request_stream << "Connection: close\r\n\r\n";
		socket.write( request_stream.str( ) );
		socket.end( );
	} ).on(  )
		
		
	socket.connect( "dynoweb.private", 80 );
	
	base::Handle::get( ).run( );

	while( true ) { }

	system( "pause" );
	return EXIT_SUCCESS;
}
