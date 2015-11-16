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

#include "lib_net_socket_boost_socket.h"

namespace nodepp {
	namespace lib {
		namespace net {
			namespace impl {
				bool is_open( boost::asio::ip::tcp::socket const & socket ) {
					return socket.is_open( );
				}

				bool is_open( boost::asio::ssl::stream<boost::asio::ip::tcp::socket> const & socket ) {
					return socket.lowest_layer( ).is_open( );
				}

				bool is_open( daw::nodepp::lib::net::RawSocket const & socket ) {
					return boost::apply_visitor( daw::make_forwarding_visitor<bool>( []( auto const & s ) {
						return is_open( s );
					} ), *socket );
				}

				boost::system::error_code shutdown( boost::asio::ip::tcp::socket & socket, boost::asio::ip::tcp::socket::shutdown_type what ) {
					socket.shutdown( what );
					return boost::system::errc::make_error_code( boost::system::errc::success );
				}

				boost::system::error_code shutdown( boost::asio::ip::tcp::socket & socket, boost::asio::ip::tcp::socket::shutdown_type what, boost::system::error_code & ec ) {
					return socket.shutdown( what, ec );
				}

				boost::system::error_code shutdown( boost::asio::ssl::stream<boost::asio::ip::tcp::socket> & socket, boost::asio::ip::tcp::socket::shutdown_type ) {
					socket.shutdown( );
					return boost::system::errc::make_error_code( boost::system::errc::success );
				}

				boost::system::error_code shutdown( boost::asio::ssl::stream<boost::asio::ip::tcp::socket> & socket, boost::asio::ip::tcp::socket::shutdown_type, boost::system::error_code & ec ) {
					return socket.shutdown( ec );
				}

				boost::system::error_code shutdown( daw::nodepp::lib::net::RawSocket & socket, boost::asio::ip::tcp::socket::shutdown_type what ) {
					return boost::apply_visitor( daw::make_forwarding_visitor<boost::system::error_code>( [&]( auto & s ) {
						return daw::nodepp::lib::net::impl::shutdown( s, what );
					} ), *socket );
				}

				boost::system::error_code shutdown( daw::nodepp::lib::net::RawSocket & socket, boost::asio::ip::tcp::socket::shutdown_type what, boost::system::error_code & ec ) {
					return boost::apply_visitor( daw::make_forwarding_visitor<boost::system::error_code>( [&]( auto & s ) {
						return daw::nodepp::lib::net::impl::shutdown( s, what, ec );
					} ), *socket );
				}

				void close( boost::asio::ip::tcp::socket & socket ) {
					socket.close( );
				}

				void close( boost::asio::ssl::stream<boost::asio::ip::tcp::socket> & socket ) {
					socket.shutdown( );
				}

				boost::system::error_code close( boost::asio::ip::tcp::socket & socket, boost::system::error_code & ec ) {
					return socket.close( ec );
				}

				boost::system::error_code close( boost::asio::ssl::stream<boost::asio::ip::tcp::socket> & socket, boost::system::error_code & ec ) {
					return socket.shutdown( ec );
				}

				void close( daw::nodepp::lib::net::RawSocket & socket ) {
					boost::apply_visitor( daw::make_forwarding_visitor<void>( []( auto & s ) {
						daw::nodepp::lib::net::impl::close( s );
					} ), *socket );
				}

				boost::system::error_code close( daw::nodepp::lib::net::RawSocket & socket, boost::system::error_code & ec ) {
					return boost::apply_visitor( daw::make_forwarding_visitor<boost::system::error_code>( [&ec]( auto & s ) {
						return daw::nodepp::lib::net::impl::close( s, ec );
					} ), *socket );
				}

				void cancel( boost::asio::ip::tcp::socket & socket ) {
					socket.cancel( );
				}

				void cancel( boost::asio::ssl::stream<boost::asio::ip::tcp::socket> & socket ) {
					socket.lowest_layer( ).cancel( );
				}

				void cancel( daw::nodepp::lib::net::RawSocket & socket ) {
					boost::apply_visitor( daw::make_forwarding_visitor<void>( [&]( auto & s ) {
						daw::nodepp::lib::net::impl::cancel( s );
					} ), *socket );
				}

				boost::asio::ip::tcp::endpoint remote_endpoint( boost::asio::ip::tcp::socket const & socket ) {
					return socket.remote_endpoint( );
				}

				boost::asio::ip::tcp::endpoint remote_endpoint( boost::asio::ssl::stream<boost::asio::ip::tcp::socket> const & socket ) {
					return socket.lowest_layer( ).remote_endpoint( );
				}

				boost::asio::ip::tcp::endpoint remote_endpoint( daw::nodepp::lib::net::RawSocket const & socket ) {
					return boost::apply_visitor( daw::make_forwarding_visitor < boost::asio::ip::tcp::endpoint>( []( auto & s ) {
						return remote_endpoint( s );
					} ), *socket );
				}

				boost::asio::ip::tcp::endpoint local_endpoint( boost::asio::ip::tcp::socket const & socket ) {
					return socket.local_endpoint( );
				}

				boost::asio::ip::tcp::endpoint local_endpoint( boost::asio::ssl::stream<boost::asio::ip::tcp::socket> const & socket ) {
					return socket.lowest_layer( ).local_endpoint( );
				}

				boost::asio::ip::tcp::endpoint local_endpoint( daw::nodepp::lib::net::RawSocket const & socket ) {
					return boost::apply_visitor( daw::make_forwarding_visitor < boost::asio::ip::tcp::endpoint>( []( auto & s ) {
						return local_endpoint( s );
					} ), *socket );
				}
			}	// namespace impl
		}	// namespace net
	}	// namespace lib
}	// namespace nodepp
}	// namespace daw