
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
	} ).when_client_connected( []( std::shared_ptr<lib::http::HttpConnection> client_connection ) {
		client_connection->when_request_made( []( std::shared_ptr<lib::http::HttpClientRequest> request, std::shared_ptr<lib::http::HttpServerResponse> response ) {
			response->send_status( 200 );
			response->add_header( "Content-Type", "text/html" );
			response->add_header( "Connection", "close" );
			response->end( "<p>Hello World</p>" );
			response->close( );
		} );
	} );
	
	server.listen_on( 8080 );

	server.run( );

	return EXIT_SUCCESS;
}
