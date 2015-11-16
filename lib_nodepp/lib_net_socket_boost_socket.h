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

#pragma once

#include <boost/asio.hpp>
#include <boost/variant.hpp>
#include <type_traits>
#include "base_types.h"

namespace daw {
	template<class Result, class Func>
	struct forwarding_visitor: boost::static_visitor<Result> {
		Func func;
		forwarding_visitor( const Func& f ):func( f ) { }
		forwarding_visitor( Func&& f ):func( std::move( f ) ) { }
		template<class Arg>
		Result operator()( Arg && arg ) const {
			return func( std::forward<Arg>( arg ) );
		}
	};

	template<class Result, class Func>
	forwarding_visitor<Result, std::decay_t<Func> > make_forwarding_visitor( Func && func ) {
		return { std::forward<Func>( func ) };
	}

	namespace nodepp {
		namespace lib {
			namespace net {
				using RawSocketValue = boost::variant<boost::asio::ip::tcp::socket, boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>;
				using RawSocket = std::shared_ptr <RawSocketValue>;

				namespace impl {
					bool is_open( boost::asio::ip::tcp::socket const & socket );
					bool is_open( boost::asio::ssl::stream<boost::asio::ip::tcp::socket> const & socket );
					bool is_open( daw::nodepp::lib::net::RawSocket const & socket );
					boost::system::error_code shutdown( boost::asio::ip::tcp::socket & socket, boost::asio::ip::tcp::socket::shutdown_type what );
					boost::system::error_code shutdown( boost::asio::ip::tcp::socket & socket, boost::asio::ip::tcp::socket::shutdown_type what, boost::system::error_code & ec );
					boost::system::error_code shutdown( boost::asio::ssl::stream<boost::asio::ip::tcp::socket> & socket, boost::asio::ip::tcp::socket::shutdown_type );
					boost::system::error_code shutdown( boost::asio::ssl::stream<boost::asio::ip::tcp::socket> & socket, boost::asio::ip::tcp::socket::shutdown_type, boost::system::error_code & ec );
					boost::system::error_code shutdown( daw::nodepp::lib::net::RawSocket & socket, boost::asio::ip::tcp::socket::shutdown_type what );
					boost::system::error_code shutdown( daw::nodepp::lib::net::RawSocket & socket, boost::asio::ip::tcp::socket::shutdown_type what, boost::system::error_code & ec );
					void close( boost::asio::ip::tcp::socket & socket );
					void close( boost::asio::ssl::stream<boost::asio::ip::tcp::socket> & socket );
					boost::system::error_code close( boost::asio::ip::tcp::socket & socket, boost::system::error_code & ec );
					boost::system::error_code close( boost::asio::ssl::stream<boost::asio::ip::tcp::socket> & socket, boost::system::error_code & ec );
					void close( daw::nodepp::lib::net::RawSocket & socket );
					boost::system::error_code close( daw::nodepp::lib::net::RawSocket & socket, boost::system::error_code & ec );
					void cancel( boost::asio::ip::tcp::socket & socket );
					void cancel( boost::asio::ssl::stream<boost::asio::ip::tcp::socket> & socket );
					void cancel( daw::nodepp::lib::net::RawSocket & socket );
					boost::asio::ip::tcp::endpoint remote_endpoint( boost::asio::ip::tcp::socket const & socket );
					boost::asio::ip::tcp::endpoint remote_endpoint( boost::asio::ssl::stream<boost::asio::ip::tcp::socket> const & socket );
					boost::asio::ip::tcp::endpoint remote_endpoint( daw::nodepp::lib::net::RawSocket const & socket );
					boost::asio::ip::tcp::endpoint local_endpoint( boost::asio::ip::tcp::socket const & socket );
					boost::asio::ip::tcp::endpoint local_endpoint( boost::asio::ssl::stream<boost::asio::ip::tcp::socket> const & socket );
					boost::asio::ip::tcp::endpoint local_endpoint( daw::nodepp::lib::net::RawSocket const & socket );

					template<typename ConstBufferSequence, typename WriteHandler>
					void async_write( daw::nodepp::lib::net::RawSocket socket, ConstBufferSequence const & buffer, WriteHandler handler ) {
						boost::apply_visitor( daw::make_forwarding_visitor<void>( [&]( auto & s ) {
							boost::asio::async_write( s, buffer, handler );
						} ), *socket );
					}

					template<typename MutableBufferSequence, typename ReadHandler>
					void async_read( daw::nodepp::lib::net::RawSocket socket, MutableBufferSequence & buffer, ReadHandler handler ) {
						boost::apply_visitor( daw::make_forwarding_visitor<void>( [&]( auto & s ) {
							boost::asio::async_read( s, buffer, handler );
						} ), *socket );
					}

					template<typename MutableBufferSequence, typename MatchType, typename ReadHandler>
					void async_read_until( daw::nodepp::lib::net::RawSocket socket, MutableBufferSequence & buffer, MatchType && m, ReadHandler handler ) {
						boost::apply_visitor( daw::make_forwarding_visitor<void>( [&]( auto & s ) {
							boost::asio::async_read_until( s, buffer, std::forward<MatchType>( m ), handler );
						} ), *socket );
					}

					template<typename Iterator, typename ComposedConnectHandler>
					void async_connect( boost::asio::ip::tcp::socket & socket, Iterator it, ComposedConnectHandler handler ) {
						boost::asio::async_connect( socket, it, handler );
					}

					template<typename Iterator, typename ComposedConnectHandler>
					void async_connect( boost::asio::ssl::stream<boost::asio::ip::tcp::socket> & socket, Iterator it, ComposedConnectHandler handler ) {
						boost::asio::async_connect( socket.lowest_layer( ), it, handler );
					}

					template<typename Iterator, typename ComposedConnectHandler>
					void async_connect( daw::nodepp::lib::net::RawSocket socket, Iterator it, ComposedConnectHandler handler ) {
						boost::apply_visitor( daw::make_forwarding_visitor<void>( [&]( auto & s ) {
							daw::nodepp::lib::net::impl::async_connect( s, it, handler );
						} ), *socket );
					}
				}	// namespace impl
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
#undef create_visitor