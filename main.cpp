

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
#include "base_service_handle.h"
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
	socket.on( "error", [&socket]( base::Error const & error ) {
		std::cerr << "Error connecting" << std::endl << error << std::endl;
		socket.destroy( );
		exit( EXIT_FAILURE );
	} ).on( "connect", [&socket]( ) {
		auto const & server_address = socket.remote_address( );
		auto requested_page = "/";
		std::cout << "Connected to " << socket.remote_address( ) << ":" << socket.remote_port( ) << std::endl;
		std::stringstream request_stream;
		request_stream << "GET " << requested_page << " HTTP/1.0\r\n";
		request_stream << "Host: " << server_address << "\r\n";
		request_stream << "Accept: */*\r\n";
		request_stream << "Connection: close\r\n\r\n";
		socket.write( request_stream.str( ) );
		socket.end( );
	} ).on( "data", []( base::data_t data_buffer ) {
		std::string buff( data_buffer.begin( ), data_buffer.end( ) );
		std::cout << buff;
	} ).on( "end", [&socket]( ) {
		auto buff( socket.read( ) );
		buff.emplace_back( 0 );
		std::cout << reinterpret_cast<char*>(buff.data( )) << std::endl;
	} ).connect( "dynoweb.private", 80 );
	base::ServiceHandle::get( ).run( );


	std::cout << "\n\nRead " << socket.bytes_read( ) << "bytes	Wrote: " << socket.bytes_written( ) << "bytes\n";
	//	system( "pause" );
	return EXIT_SUCCESS;
}
