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

#pragma once

#include <boost/asio.hpp>
#include <cassert>
#include "base_types.h"

namespace daw {
	template<class Result, class Func>
	struct forwarding_visitor final: boost::static_visitor<Result> {
		Func func;
		forwarding_visitor( const Func& f ):func( f ) { }
		forwarding_visitor( Func&& f ):func( std::move( f ) ) { }
		~forwarding_visitor( ) = default;
		forwarding_visitor( forwarding_visitor const & ) = default;
		forwarding_visitor( forwarding_visitor && ) = default;
		forwarding_visitor & operator=( forwarding_visitor const & ) = default;
		forwarding_visitor & operator=( forwarding_visitor && ) = default;

		template<class Arg>
		Result operator()( Arg && arg ) const {
			return func( std::forward<Arg>( arg ) );
		}
	};

	template<class Result, class Func>
	forwarding_visitor<Result, std::decay_t<Func>> make_forwarding_visitor( Func && func ) {
		return { std::forward<Func>( func ) };
	}

	namespace nodepp {
		namespace lib {
			namespace net {
				namespace impl {
					using EncryptionContext = boost::asio::ssl::context;
					struct BoostSocket final {
						using BoostSocketValueType = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;
						std::shared_ptr<EncryptionContext> m_encryption_context;
						bool m_encryption_enabled;
					private:
						std::shared_ptr <BoostSocketValueType> m_socket;
						BoostSocketValueType & raw_socket( );
						BoostSocketValueType const & raw_socket( ) const;
						
					public:
						BoostSocket( ) = delete;
						~BoostSocket( ) = default;
						BoostSocket( BoostSocket const & ) = default;
						BoostSocket& operator=( BoostSocket const & ) = default;
						BoostSocket( BoostSocket && ) = default;
						BoostSocket& operator=( BoostSocket && ) = default;						

						explicit operator bool( ) const;

						void init( );

						BoostSocket( std::shared_ptr<EncryptionContext> context );
						BoostSocket( std::shared_ptr<BoostSocketValueType> socket, std::shared_ptr<EncryptionContext> context );

						BoostSocketValueType const & operator*( ) const;
						BoostSocketValueType & operator*( );
						BoostSocketValueType * operator->( ) const;
						BoostSocketValueType * operator->( );

						bool encyption_on( ) const;
						bool & encyption_on( );
						void encyption_on( bool value );
						void reset_socket( );

						EncryptionContext & encryption_context( );
						EncryptionContext const & encryption_context( ) const;

						bool is_open( ) const;
						void shutdown( boost::asio::ip::tcp::socket::shutdown_type );
						boost::system::error_code shutdown( boost::asio::ip::tcp::socket::shutdown_type, boost::system::error_code & ec );
						void close( );
						boost::system::error_code close( boost::system::error_code & ec );
						void cancel( );
						boost::asio::ip::tcp::endpoint remote_endpoint( ) const;
						boost::asio::ip::tcp::endpoint local_endpoint( ) const;

						template<typename HandshakeHandler>
						void async_handshake( BoostSocketValueType::handshake_type role, HandshakeHandler handler ) {
							assert( static_cast<bool>(m_socket) );
							if( encyption_on( ) ) {
								return;
							}
							m_socket->async_handshake( role, handler );
						}

						template<typename ConstBufferSequence, typename WriteHandler>
						void async_write( ConstBufferSequence const & buffer, WriteHandler handler ) {
							if( encyption_on( ) ) {
								boost::asio::async_write( *m_socket, buffer, handler );
							} else {
								boost::asio::async_write( m_socket->next_layer( ), buffer, handler );
							}
						}

						template<typename MutableBufferSequence, typename ReadHandler>
						void async_read( MutableBufferSequence & buffer, ReadHandler handler ) {
							if( encyption_on( ) ) {
								boost::asio::async_read( *m_socket, buffer, handler );
							} else {
								boost::asio::async_read( m_socket->next_layer( ), buffer, handler );
							}
						}

						template<typename MutableBufferSequence, typename MatchType, typename ReadHandler>
						void async_read_until( MutableBufferSequence & buffer, MatchType && m, ReadHandler handler ) {
							if( encyption_on( ) ) {
								boost::asio::async_read_until( *m_socket, buffer, std::forward<MatchType>( m ), handler );
							} else {
								boost::asio::async_read_until( m_socket->next_layer( ), buffer, std::forward<MatchType>( m ), handler );
							}
						}

						template<typename Iterator, typename ComposedConnectHandler>
						void async_connect( Iterator it, ComposedConnectHandler handler ) {
							boost::asio::async_connect( m_socket->next_layer( ), it, handler );
						}

						void enable_encryption( boost::asio::ssl::stream_base::handshake_type handshake );
					};

					BoostSocket create_boost_socket( boost::asio::io_service & io_service, std::shared_ptr<EncryptionContext> context );
				}	// namespace impl
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
// TOOD remove #undef create_visitor

