
#include <cstdlib>
#include <memory>

#include "lib_http_request.h"
#include "lib_http_site.h"

int main( int, char const ** ) {
	using namespace daw::nodepp;
	using namespace lib::http;

	auto site = create_http_site( );
	site->on_requests_for( HttpClientRequestMethod::Get, "/", [&]( HttpClientRequest request, HttpServerResponse response ) {
		response->on_all_writes_completed( [response]( ) mutable {
			response->close( );
		} ).send_status( 200 )
			.add_header( "Content-Type", "text/html" )
			.add_header( "Connection", "close" )
			.end( R"(<p>Hello World!</p>)" );
	} ).on_error( []( base::Error error ) {
		std::cerr << error << std::endl;
	} ).listen_on( 8080 );


	base::start_service( base::StartServiceMode::Single );

	return EXIT_SUCCESS;
}
