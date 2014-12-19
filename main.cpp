
#include <cstdlib>
#include <memory>

#include "lib_http_request.h"
#include "lib_http_server.h"
#include "lib_http_server_response.h"

int main( int, char const ** ) {
	using namespace daw::nodepp;

	auto server = lib::http::HttpServer( );
	server.when_listening( []( boost::asio::ip::tcp::endpoint endpoint ) {
		std::cout << "Server listening on " << endpoint << "\n";
	} ).when_client_connected( []( lib::http::HttpConnection client_connection ) {
		client_connection.when_request_made( []( std::shared_ptr<lib::http::HttpClientRequest> request, lib::http::HttpServerResponse response ) {
			response.when_all_writes_complete( [response]( ) mutable { 
				response.close( );
			} );
			response.send_status( 200 );
			response.add_header( "Content-Type", "text/html" );
			response.add_header( "Connection", "close" );
			response.end( R"(<p>Hello World!</p>)" );
		} );
	} );

	server.listen_on( 8080 );

	server.run( );

	return EXIT_SUCCESS;
}
