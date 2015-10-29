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
#include "daw_json.h"
#include "lib_http_request.h"
#include "lib_http_site.h"
#include "lib_http_webservice.h"
#include "lib_net_server.h"
#include "parse_json/daw_json_link.h"
#include <boost/utility/string_ref.hpp>
#include <boost/filesystem.hpp>

template<typename Container, typename T>
void if_exists_do( Container & container, T const & key, std::function<void( typename Container::iterator it )> action ) {
	auto it = std::find( std::begin( container ), std::end( container ), key );
	if( std::end( container ) != it ) {
		action( it );
	}
}

bool begins_with_nc( boost::string_ref a, boost::string_ref b ) {
	if( a.size( ) > b.size( ) ) {
		return false;
	}
	for( size_t n = 0; n < a.size( ); ++n ) {
		auto const u_a = (a[n] | 0x20);
		auto const u_b = (b[n] | 0x20);
		if( u_a != u_b ) {
			return false;
		}
	}
	return true;
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
		ss << ex.what( ) << '\n';
	}

	return ss.str( );
}

struct locked_buffer {
	using buffers_t = std::list<std::shared_ptr<boost::asio::streambuf>>;
	buffers_t data;
	std::mutex data_mutex;
};

int main( int, char const ** ) {
	using namespace daw::nodepp;
	using namespace daw::nodepp::lib::net;

	locked_buffer buffers;

	auto srv = create_net_server( );

	srv->on_connection( [&]( NetSocketStream socket ) {
		std::cout << "Connection from: " << socket->remote_address( ) << ":" << socket->remote_port( ) << std::endl;
		locked_buffer::buffers_t::iterator it;
		{
			std::unique_lock<std::mutex> lock( buffers.data_mutex );
			it = buffers.data.emplace( buffers.data.end( ), std::make_shared<boost::asio::streambuf>( 4096 ) );
		}

		socket->on_data_received( [&, socket, it]( std::shared_ptr<base::data_t> data_buffer, bool ) mutable {
			if( data_buffer ) {
				std::string const msg { data_buffer->begin( ), data_buffer->end( ) };
				if( begins_with_nc( "quit", msg ) ) {
					socket->on_all_writes_completed( [&, socket]( ) {
						socket->close( );
					} ).write_async( "GOOD-BYTE" );
				} else if( begins_with_nc( "dir", msg ) ) {
					socket->write_async( get_directory_listing( "." ) + "\r\nREADY\r\n" );
				} else if( begins_with_nc( "help", msg ) ) {
				} else {
					socket->write_async( "SYNTAX ERROR\r\n" );
				}
			}
			socket->read_async( );
		} ).set_read_mode( daw::nodepp::lib::net::impl::NetSocketStreamImpl::ReadUntil::newline )
//			.on_closed( [&, it]( ) mutable {
// 			std::unique_lock<std::mutex> lock( buffers.data_mutex );
// 			if( buffers.data.end( ) != it ) {
// 				buffers.data.erase( it );
// 			}
/*} )*/.write_async( "READY\r\n" );
		socket->read_async( );
	} );

	srv->listen( 2020 );

	base::ServiceHandle::run( );
	return EXIT_SUCCESS;
}