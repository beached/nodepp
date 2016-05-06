// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Darrell Wright
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

#include <daw/nodepp/base_work_queue.h>
#include <daw/nodepp/lib_net_server.h>
#include <daw/nodepp/lib_net_socket_stream.h>
#include <boost/utility/string_ref.hpp>
#include <boost/filesystem.hpp>
#include <daw/daw_string.h>
#include <boost/program_options.hpp>

std::string get_directory_listing( boost::string_ref folder );

int main( int argc, char const ** argv ) {
	using namespace daw::nodepp;
	using namespace daw::nodepp::lib::net;
	namespace po = boost::program_options;
	uint16_t port;
	std::string cert = "";
	std::string key = "";

	po::options_description desc( "Allowed options" );
	desc.add_options( )
		("help", "produce help message")
		("port", po::value<uint16_t>( &port )->default_value( 2020 ), "port to listen on")
		("cert", po::value<std::string>( &cert )->default_value( "" ), "The certificate chain file")
		("key", po::value<std::string>( &key )->default_value( "" ), "The certificate private key file");

	po::variables_map vm;
	po::store( po::parse_command_line( argc, argv, desc ), vm );
	po::notify( vm );

	if( vm.count( "help" ) ) {
		std::cout <<desc <<std::endl;
		return EXIT_SUCCESS;
	}

	daw::nodepp::lib::net::NetServer srv = create_net_server( boost::asio::ssl::context::tlsv12_server );

	if( !(cert.empty( ) || key.empty( )) ) {
		auto & ctx = srv->ssl_context( );
		using namespace boost::asio::ssl;
		ctx.set_options( context::default_workarounds | boost::asio::ssl::context::single_dh_use );
		ctx.use_certificate_chain_file( cert.c_str( ) );
		ctx.use_private_key_file( key.c_str( ), context::pem );
	}
	srv->on_connection( [&]( NetSocketStream socket ) {
		std::cout <<"Connection from: " <<socket->remote_address( ) <<":" <<socket->remote_port( ) <<std::endl;

		socket->on_data_received( [obj=socket->get_weak_ptr( )]( std::shared_ptr<daw::nodepp::base::data_t> data_buffer, bool ) mutable {

			if( auto sck = obj.lock( ) ) {
				if( data_buffer ) {
					auto const msg = daw::AsciiLower( daw::string::trim_copy( std::string { data_buffer->begin( ), data_buffer->end( ) } ) );
					if( msg == "quit" ) {
						sck->end( "GOOD-BYTE\r\n" );
					} else if( msg == "dir" ) {
						sck << get_directory_listing( "." ) << "\r\nREADY\r\n";
					} else if( msg == "help" ) {
						sck << "quit - close connection\r\n";
						sck << "dir - show directory listing\r\n";
						sck << "help - this message\r\n";
						sck << "READY\r\n";
					} else if( msg == "starttls" ) {
						NetSocketStream local_socket = sck;
						sck->socket( ).async_handshake( impl::BoostSocket::BoostSocketValueType::handshake_type::server, [local_socket]( auto const & error ) mutable {
							if( error ) {
								std::cerr << "Error starting encryption: " << error << ": " << error.message( ) << std::endl;
								return;
							}
							local_socket << "Encryption enabled\r\n";
						} );
					} else {
						sck << "SYNTAX ERROR\r\n\nREADY\r\n";
					}
				}
			}
		} ).on_error( []( auto error ) {
			std::cerr <<"Error while serving: " <<error <<std::endl;
		} ).set_read_mode( NetSocketStreamReadMode::newline );	// Every time a newline is received, on_data_received callback is called

		socket <<"READY\r\n";

		socket->read_async( );
	} );

	srv->listen( port );

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
				ss <<p <<" size is " <<fs::file_size( p ) <<"\r\n";
			} else if( fs::is_directory( p ) ) {
				ss <<p <<" is a directory containing:\n";
				std::copy( fs::directory_iterator( p ), fs::directory_iterator( ), std::ostream_iterator<fs::directory_entry>( ss, "\r\n" ) );
			} else {
				ss <<p <<" exists, but is neither a regular file nor a directory\n";
			}
		} else {
			ss <<p <<" does not exist\n";
		}
	}

	catch( const fs::filesystem_error& ex ) {
		ss <<"ERROR: " <<ex.what( ) <<'\n';
	}

	return ss.str( );
}

