

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
#include "base_url.h"
#include "lib_http.h"
#include "lib_http_client_request.h"
#include "lib_http_server.h"
#include "lib_http_server_response.h"
#include "lib_net_dns.h"
#include "lib_net_server.h"
#include "lib_net_socket.h"
#include "range_algorithm.h"
#include "range_algorithm.h"
#include "utility.h"

int main( int, char const ** ) {
	using namespace daw::nodepp;
	
	//auto server = lib::http::HttpServer( );
	auto server = lib::http::HttpServer( );
	server.on_listening( []( boost::asio::ip::tcp::endpoint endpoint ) {
		std::cout << "Server listening on " << endpoint << "\n";
// 		response.status = lib::http::HttpStatusCodes( 200 );
// 		response.headers["Content-Type"] = "text/plain";
//		response.write( "Hello World" );
	} ).on_connection( []( lib::http::HttpConnection & con ) {
		con.on_requestGet( []( lib::http::HttpClientRequest request, lib::http::HttpServerResponse response ) {
			std::cout << "GET request";
		} );
	} );
	
	server.listen( 8080 );

	server.run( );


//	std::cout << "\n\nRead " << socket.bytes_read( ) << "bytes	Wrote: " << socket.bytes_written( ) << "bytes\n";
	//	system( "pause" );
	return EXIT_SUCCESS;
}
