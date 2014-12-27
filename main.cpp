
#include <cstdlib>
#include <memory>

#include "lib_http_request.h"
#include "lib_http_site.h"

int main( int, char const ** ) {
	using namespace daw::nodepp;
	using namespace lib::http;

// 	auto server = lib::http::create_http_server( );
// 	
// 	server->on_listening( []( boost::asio::ip::tcp::endpoint endpoint ) {
// 		std::cout << "Server listening on " << endpoint << "\n";
// 	} ).on_client_connected( []( lib::http::HttpConnection client_connection ) {
// 		client_connection->on_request_made( []( lib::http::HttpClientRequest request, lib::http::HttpServerResponse response ) {
// 			if( request->request.url == "/" && request->request.method == lib::http::HttpClientRequestMethod::Get ) {
// 				response->on_all_writes_completed( [response]( ) mutable {
// 					response->close( );
// 				} ).send_status( 200 )
// 					.add_header( "Content-Type", "text/html" )
// 					.add_header( "Connection", "close" )
// 					.end( R"(<p>Hello World!</p>)" );
// 			} else {
// 				response->on_all_writes_completed( [response]( ) mutable {
// 					response->close( );
// 				} ).send_status( 404 )
// 					.add_header( "Content-Type", "text/html" )
// 					.add_header( "Connection", "close" )
// 					.end( R"(<p>Resource not found</p>)" );
// 			}
// 			} );
// 		} ).on_error( []( base::Error error ) {
// 		std::cerr << error << std::endl;
// 	} ).listen_on( 8080 );

	auto site = create_http_site( );
	site->create_path( HttpClientRequestMethod::Get, "/", [&]( HttpClientRequest request, HttpServerResponse response ) {
		response->on_all_writes_completed( [response]( ) mutable {
			response->close( );
		} ).send_status( 200 )
			.add_header( "Content-Type", "text/html" )
			.add_header( "Connection", "close" )
			.end( R"(<p>Hello World!</p>)" );
	} );
	
	site->on_error( []( base::Error error ) {
		std::cerr << error << std::endl;
	} ).listen( 8080 );


	base::ServiceHandle::run( );

	return EXIT_SUCCESS;
}
