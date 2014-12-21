
#include <cstdlib>
#include <memory>

#include "lib_http_request.h"
#include "lib_http_server.h"
#include "lib_http_server_response.h"

int main( int, char const ** ) {
	using namespace daw::nodepp;

	auto server = lib::http::HttpServer( );
	
	server.on_listening( []( boost::asio::ip::tcp::endpoint endpoint ) {
		std::cout << "Server listening on " << endpoint << "\n";
	} );

	server.on_client_connected( []( lib::http::HttpConnection client_connection ) {
		client_connection->on_request_made( []( std::shared_ptr<lib::http::HttpClientRequest> request, lib::http::HttpServerResponse response ) {
			response->on_all_writes_completed( [response]( ) mutable { 
				response->close( );
			} );
			response->send_status( 200 );
			response->add_header( "Content-Type", "text/html" );
			response->add_header( "Connection", "close" );
			response->end( R"(<p>Hello World!</p>)" );
		} );
	} );

	server.on_error( []( base::Error error ) {
		std::cerr << error << std::endl;
	} );

	server.listen_on( 8080 );

	base::ServiceHandle::run( );

	return EXIT_SUCCESS;
}
