
#include <cstdlib>
#include <memory>

#include "lib_http_request.h"
#include "lib_http_site.h"
#include "base_work_queue.h"

int main( int, char const ** ) {
	using namespace daw::nodepp;
	using namespace lib::http;

	auto site = create_http_site( );
	site->on_listening( []( boost::asio::ip::tcp::endpoint endpoint ) {
		std::cout << "Listening on " << endpoint << "\n";
	} ).on_requests_for( HttpClientRequestMethod::Get, "/", [&]( HttpClientRequest request, HttpServerResponse response ) {
		response->on_all_writes_completed( [response]( ) mutable {
			response->close( );
		} ).send_status( 200 )
			.add_header( "Content-Type", "text/html" )
			.add_header( "Connection", "close" )
			.end( R"(<p>Hello World!</p>)" );
	} ).on_error( []( base::Error error ) {
		std::cerr << error << std::endl;
	} ).listen_on( 8080 ).on_page_error( 404, []( lib::http::HttpClientRequest request, lib::http::HttpServerResponse response, uint16_t error_no ) { 
		std::cout << "404 Request for " << request->request.url.path << "with query ";
		for( auto const & p : request->request.url.query ) {
			std::cout << "{ " << p.first << "," << p.second.get_value_or( "" ) << " } ";
		}
		std::cout << "\n";
	} );

	base::ServiceHandle::run( );

	
	return EXIT_SUCCESS;
}
