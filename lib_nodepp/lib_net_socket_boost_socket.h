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
				using BoostSocket = std::shared_ptr <boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>;

				namespace impl {
					bool is_open( BoostSocket const & socket );
					boost::system::error_code shutdown( BoostSocket socket, boost::asio::ip::tcp::socket::shutdown_type );
					boost::system::error_code shutdown( BoostSocket socket, boost::asio::ip::tcp::socket::shutdown_type, boost::system::error_code & ec );
					void close( BoostSocket socket );
					boost::system::error_code close( BoostSocket socket, boost::system::error_code & ec );
					void cancel( BoostSocket socket );
					boost::asio::ip::tcp::endpoint remote_endpoint( BoostSocket & socket );
					boost::asio::ip::tcp::endpoint local_endpoint( BoostSocket & socket );

					template<typename HandshakeHandler>
					void async_handshake( BoostSocket & socket, bool const use_ssl, boost::asio::stream::handshake_type role, HandshakeHandler handler ) {
						if( !use_ssl ) {
							return;
						}
						socket->async_handshake( *socket, role, handler );
					}

					template<typename ConstBufferSequence, typename WriteHandler>
					void async_write( BoostSocket & socket, bool const use_ssl, ConstBufferSequence const & buffer, WriteHandler handler ) {
						if( use_ssl ) {
							boost::asio::async_write( *socket, buffer, handler );
						} else {
							boost::asio::async_write( socket->lowest_layer( ), buffer, handler );
						}
					}

					template<typename MutableBufferSequence, typename ReadHandler>
					void async_read( BoostSocket & socket, bool const use_ssl, MutableBufferSequence & buffer, ReadHandler handler ) {
						if( use_ssl ) {
							boost::asio::async_read( *socket, buffer, handler );
						} else {
							boost::asio::async_read( socket->lowest_layer( ), buffer, handler );
						}
					}

					template<typename MutableBufferSequence, typename MatchType, typename ReadHandler>
					void async_read_until( BoostSocket & socket, bool const use_ssl, MutableBufferSequence & buffer, MatchType && m, ReadHandler handler ) {
						if( use_ssl ) {
							boost::asio::async_read_until( *socket, buffer, std::forward<MatchType>( m ), handler );
						} else {
							boost::asio::async_read_until( socket->lowest_layer( ), buffer, std::forward<MatchType>( m ), handler );
						}
					}

					template<typename Iterator, typename ComposedConnectHandler>
					void async_connect( boostSocket & socket, Iterator it, ComposedConnectHandler handler ) {
						boost::asio::async_connect( socket.lowest_layer( ), it, handler );
					}
				}	// namespace impl
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
#undef create_visitor