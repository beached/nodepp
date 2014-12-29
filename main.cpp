
#include <cstdlib>
#include <memory>

#include "lib_http_request.h"
#include "lib_http_site.h"
#include "base_work_queue.h"

int main( int, char const ** ) {
	using namespace daw::nodepp;
	using namespace lib::http;

// 	auto site = create_http_site( );
// 	site->on_requests_for( HttpClientRequestMethod::Get, "/", [&]( HttpClientRequest request, HttpServerResponse response ) {
// 		response->on_all_writes_completed( [response]( ) mutable {
// 			response->close( );
// 		} ).send_status( 200 )
// 			.add_header( "Content-Type", "text/html" )
// 			.add_header( "Connection", "close" )
// 			.end( R"(<p>Hello World!</p>)" );
// 	} ).on_error( []( base::Error error ) {
// 		std::cerr << error << std::endl;
// 	} ).listen_on( 8080 );


	auto q = base::create_work_queue( );

	for( auto n = 0; n < q->max_conncurrent( ) * 2; ++n ) {
		q->add_work_item( []( int64_t task_id ) {
			std::cout << "Starting task: " << task_id << "\n";
			std::this_thread::sleep_for( std::chrono::seconds( 5 ) );
		}, []( int64_t task_id, base::OptionalError error ) {
			std::cout << "Completed task: " << task_id << "\n";
		}, false );
	}
		
	q->run( );

	boost::asio::io_service::work work( base::ServiceHandle::get( ) );
	base::ServiceHandle::run( );
	q->wait( );

	
	

	system( "Pause" );
	return EXIT_SUCCESS;
}
