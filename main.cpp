

#include "lib_http.h"

using namespace daw::nodepp::lib;

int main( int, char const ** ) {
	
	auto server = http::create_server( []( http::Request req, http::Response resp ) {

	} ).listen( 8080 );

	system("pause");
	return EXIT_SUCCESS;
}
