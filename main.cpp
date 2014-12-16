

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
#include "base_error.h"
#include "lib_http.h"
#include "lib_http_server.h"
#include "lib_http_server_response.h"
#include "lib_net_dns.h"
#include "lib_net_server.h"
#include "lib_net_socket_stream.h"
#include "range_algorithm.h"
#include "range_algorithm.h"
#include "utility.h"

int main( int, char const ** ) {
	using namespace daw::nodepp;
	
	//auto server = lib::http::HttpServer( );
	auto server = lib::http::HttpServer( );
	server.on_listening( []( boost::asio::ip::tcp::endpoint endpoint ) {
		std::cout << "Server listening on " << endpoint << "\n";
	} ).on_connection( []( std::shared_ptr<lib::http::HttpConnection> con ) {
		con->on_request( []( std::shared_ptr<lib::http::HttpClientRequest> request, std::shared_ptr<lib::http::HttpServerResponse> response ) {
			response->send_status( 200 );
			response->headers( ).add( "Content-Type", "text/html" );
			response->headers( ).add( "Connection", "close" );
			static const std::string msg = "<p>Hello World</p>";
			response->end( msg );
			response->close( );
		} );
	} );
	
	server.listen( 8080 );

	server.run( );

	return EXIT_SUCCESS;
}
