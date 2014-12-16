

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

template<typename MapKey, typename MapValue>
std::ostream &operator<<(std::ostream &stream, const std::map<MapKey, MapValue>& map) {
	for( auto it = map.begin( ); it != map.end( ); ++it ) {
		stream << "{ " << (*it).first << ", " << (*it).second << " }" << std::endl;
	}
	return stream;
}

int main( int, char const ** ) {
	using namespace daw::nodepp;
	
	//auto server = lib::http::HttpServer( );
	auto server = lib::http::HttpServer( );
	server.on_listening( []( boost::asio::ip::tcp::endpoint endpoint ) {
		std::cout << "Server listening on " << endpoint << "\n";
	} ).on_connection( []( std::shared_ptr<lib::http::HttpConnection> con ) {
		con->on_requestGet( []( lib::http::HttpClientRequest request, std::shared_ptr<lib::http::HttpServerResponse> response_ptr ) {
			response_ptr->send_status( 200 );
			response_ptr->headers( ).add( "Content-Type", "text/html" );
			response_ptr->headers( ).add( "Connection", "close" );			
			response_ptr->write( "<html><head><title>welcome</title></head><body><h1>Welcome!</h1></body><html>\n" );
			response_ptr->send( );
			response_ptr->end( );
		} );
	} ).on_error( [&]( base::Error err ) {
		while( err.has_exception( ) ) {
			try {
				err.throw_exception( );
			} catch( std::exception const & ex ) {
				std::cerr << "Exception thrown: " << ex.what( ) << std::endl;
			}
		}
		std::cout << "\n" << err << std::endl;
	} );
	
	server.listen( 8080 );

	server.run( );


//	std::cout << "\n\nRead " << socket.bytes_read( ) << "bytes	Wrote: " << socket.bytes_written( ) << "bytes\n";
	//	system( "pause" );
	return EXIT_SUCCESS;
}
