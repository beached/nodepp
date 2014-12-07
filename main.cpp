

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
#include "base_types.h"
#include "lib_http.h"
#include "lib_http_server.h"
#include "lib_net_dns.h"
#include "lib_net_socket.h"
#include "lib_net_server.h"
#include "range_algorithm.h"
#include "range_algorithm.h"
#include "utility.h"
#include "base_url.h"

int main( int, char const ** ) {
	using namespace daw::nodepp;
	// 	using listen_t = std::function < void( lib::http::HttpClientRequest, lib::http::HttpServerResponse ) > ;
	// 	auto server = lib::http::create_server( []( lib::http::HttpClientRequest request, lib::http::HttpServerResponse response ) {
	// 		response.write_head( 200, "", { { "Content-Type", "text/plain" } } );
	// 		response.write( "Hello World" );
	// 		response.end( );
	// 	} ).listen( 8080 );

	size_t count = 0;

	auto pred = [&count]( lib::net::NetSocket::match_iterator_t first, lib::net::NetSocket::match_iterator_t last ) -> std::pair < lib::net::NetSocket::match_iterator_t, bool > {
		return{ last, true };
	};

	auto server = lib::net::NetServer( );
	server.on_error( [&]( base::Error error ) {
		std::cerr << "Error connecting" << std::endl << error << std::endl;
		exit( EXIT_FAILURE );
	} ).on_connection( [&pred]( std::shared_ptr<lib::net::NetSocket> socket ) {	
		socket->set_read_predicate( pred );
		std::cout << "Connection from " << socket->remote_address( ) << std::endl;
		socket->write( "Go Away\r\n\r\n" );
		socket->on_data( []( std::shared_ptr<daw::nodepp::base::data_t> data_buffer, bool ) {
			std::string buff( data_buffer->begin( ), data_buffer->end( ) );
			std::cout << buff;
		} ).on_end( []( ) {
			std::cout << "\n\n" << std::endl;
		} );
	} ).listen( 8080 );
	
	base::ServiceHandle::get( ).run( );


//	std::cout << "\n\nRead " << socket.bytes_read( ) << "bytes	Wrote: " << socket.bytes_written( ) << "bytes\n";
	//	system( "pause" );
	return EXIT_SUCCESS;
}
