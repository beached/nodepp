// The MIT License (MIT)
//
// Copyright (c) 2014-2015 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cstdlib>
#include <memory>
#include <iostream>

#include "daw_json.h"
#include "lib_http_client.h"

int main( int, char ** ) {
	using namespace daw::nodepp::lib::http;
	auto client = create_http_client( );

	client->on_connection( []( HttpClientConnection connection ) {
		connection->on_response_returned( []( HttpServerResponse response ) {
			if( response ) {
				std::cout << boost::string_ref( response->body( ).data( ), response->body( ).size( ) ).to_string( ) << std::endl;
			}
		} );
	} );

	auto request = create_http_client_request( "/", HttpClientRequestMethod::Get );

	client->request( "http", "dawdevel.ca", 80, request );

	start_service( base::StartServiceMode::Single );
	system( "pause" );
	return EXIT_SUCCESS;
}