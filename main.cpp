

// #include "lib_http.h"
// 
// using namespace daw::nodepp::lib;

#include <boost/any.hpp>
#include <cstdint>
#include <functional>
#include <iostream>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "base_event_emitter.h"
#include "base_handle.h"
#include "lib_http.h"
#include "lib_http_server.h"
#include "range_algorithm.h"
#include "range_algorithm.h"
#include "utility.h"
#include "lib_net_dns.h"
#include <thread>

int main( int, char const ** ) {
	using namespace daw::nodepp;
// 	using listen_t = std::function < void( lib::http::HttpClientRequest, lib::http::HttpServerResponse ) > ;
// 	auto server = lib::http::create_server( []( lib::http::HttpClientRequest request, lib::http::HttpServerResponse response ) {
// 		response.write_head( 200, "", { { "Content-Type", "text/plain" } } );
// 		response.write( "Hello World" );
// 		response.end( );
// 	} ).listen( 8080 );

	auto dns = lib::net::NetDns( );
	dns.resolve( "www.google.ca", []( boost::system::error_code const & err, boost::asio::ip::tcp::resolver::iterator it ) {
		boost::asio::ip::tcp::resolver::iterator end;
		for( ; it != end; ++it ) {
			boost::asio::ip::tcp::endpoint endpoint = *it;
			std::cout << endpoint << std::endl;
		}
	} );


	auto& handle = base::Handle::get( );

	handle.run( );

	system( "pause" );
	return EXIT_SUCCESS;
}
