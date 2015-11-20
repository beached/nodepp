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

#include "base_work_queue.h"
#include "lib_net_server.h"
#include <boost/utility/string_ref.hpp>
#include <boost/filesystem.hpp>
#include "daw_string.h"

std::string get_directory_listing( boost::string_ref folder );

int main( int, char const ** ) {
	using namespace daw::nodepp;
	using namespace daw::nodepp::lib::net;

	auto srv = create_net_server( boost::asio::ssl::context::tlsv12_server );

	srv->on_connection( [&]( NetSocketStream socket ) {
		std::cout << "Connection from: " << socket->remote_address( ) << ":" << socket->remote_port( ) << std::endl;

		socket->on_data_received( [socket]( std::shared_ptr<base::data_t> data_buffer, bool ) mutable {
			if( data_buffer ) {
				auto const msg = daw::AsciiLower( daw::string::trim_copy( std::string { data_buffer->begin( ), data_buffer->end( ) } ) );
				if( msg == "quit" ) {
					socket->end( "GOOD-BYTE\r\n" );
				} else if( msg == "dir" ) {
					socket << get_directory_listing( "." ) << "\r\nREADY\r\n";
				} else if( msg == "help" ) {
					socket << "quit - close connection\r\n";
					socket << "dir - show directory listing\r\n";
					socket << "help - this message\r\n";
					socket << "READY\r\n";
				} else {
					socket << "SYNTAX ERROR\r\n\nREADY\r\n";
				}
			}
		} ).set_read_mode( NetSocketStreamReadMode::newline );	// Every time a newline is received, on_data_received callback is called

		socket << "READY\r\n";

		socket->read_async( );
	} );

	srv->listen( 2020 );

	base::start_service( base::StartServiceMode::Single );

	return EXIT_SUCCESS;
}

std::string get_directory_listing( boost::string_ref folder ) {
	namespace fs = boost::filesystem;
	fs::path p { folder.to_string( ) };
	std::ostringstream ss;
	try {
		if( exists( p ) ) {
			if( fs::is_regular_file( p ) ) {
				ss << p << " size is " << fs::file_size( p ) << "\r\n";
			} else if( fs::is_directory( p ) ) {
				ss << p << " is a directory containing:\n";
				std::copy( fs::directory_iterator( p ), fs::directory_iterator( ), std::ostream_iterator<fs::directory_entry>( ss, "\r\n" ) );
			} else {
				ss << p << " exists, but is neither a regular file nor a directory\n";
			}
		} else {
			ss << p << " does not exist\n";
		}
	}

	catch( const fs::filesystem_error& ex ) {
		ss << "ERROR: " << ex.what( ) << '\n';
	}

	return ss.str( );
}