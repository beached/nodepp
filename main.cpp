

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
	
	auto server = lib::http::HttpServer( );
	
	server.on_listening( [&]( lib::http::HttpClientRequest, lib::http::HttpServerResponse response ) {
		response.write_head( 200, "", { { "Content-Type", "text/plain" } } );
		response.end( "Hello World" );
	} );
	
	server.listen( 8080 );

	base::ServiceHandle::get( ).run( );


//	std::cout << "\n\nRead " << socket.bytes_read( ) << "bytes	Wrote: " << socket.bytes_written( ) << "bytes\n";
	//	system( "pause" );
	return EXIT_SUCCESS;
}
