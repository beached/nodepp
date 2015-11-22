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
#include <iostream>
#include <boost/program_options.hpp>
#include "lib_net_socket_stream.h"

int main( int argc, char const ** argv ) {
	using namespace daw::nodepp;
	using namespace daw::nodepp::lib::net;
	namespace po = boost::program_options;

	auto has_directory = false;

	uint16_t port;
	std::string host_name;
	std::string ca_cert;

	po::options_description desc( "Allowed options" );
	desc.add_options( )
		("help", "produce help message")
		("port", po::value<uint16_t>( &port )->default_value( 2020 ), "port to connect to")
		("key", po::value<std::string>( &host_name )->default_value( "" ), "The host to connect to")
		("ca_cert", po::value<std::string>( &ca_cert )->default_value( "" ), "The certificate to verify server cert");

	po::variables_map vm;
	po::store( po::parse_command_line( argc, argv, desc ), vm );
	po::notify( vm );

	auto socket = create_net_socket_stream( boost::asio::ssl::context::tlsv12_client );
	if( !ca_cert.empty( ) ) {
		boost::asio::ssl::context & ctx = *(socket->socket( ).encryption_context);
		ctx.load_verify_file( ca_cert.c_str( ) );
	}
	socket->on_connected( [socket]( ) mutable {
		std::cout << "Connection from: " << socket->remote_address( ) << ":" << socket->remote_port( ) << std::endl;
		socket->read_async( );
	} ).on_data_received( [&has_directory, socket]( auto data_buffer, bool ) mutable {
		if( data_buffer ) {
			auto const msg = std::string { data_buffer->begin( ), data_buffer->end( ) };
			std::cout << msg;
			if( has_directory ) {
				socket->end( "quit\r\n" );

				socket->close( );
			} else {
				has_directory = true;
				socket << "dir\r\n";
			}
		}
	} ).set_read_until_values( "READY\r\n", false );

	socket->connect( host_name.c_str( ), port );

	base::start_service( base::StartServiceMode::Single );
	return EXIT_SUCCESS;
}