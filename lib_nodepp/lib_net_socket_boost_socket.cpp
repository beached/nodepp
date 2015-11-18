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

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				namespace impl {
					BoostSocket::BoostSocket( std::shared_ptr<BoostSocket::BoostSocketValueType> socket, bool use_ssl ): m_socket( std::move( socket ) ), m_use_ssl( use_ssl ) { }

					BoostSocket create_boost_socket( boost::asio::io_service & io_service, boost::asio::ssl::context::method ctx_method, bool use_ssl ) {
						return BoostSocket( std::make_shared < boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>( io_service, ctx_method ), use_ssl );
					}

					bool BoostSocket::use_ssl( ) const {
						return m_use_ssl;
					}

					bool & BoostSocket::use_ssl( ) {
						return m_use_ssl;
					}

					void BoostSocket::use_ssl( bool && value ) {
						m_use_ssl = std::forward<bool>( value );
					}

					BoostSocket::BoostSocketValueType const & BoostSocket::operator*( ) const {
						assert( static_cast<bool>(m_socket) );
						return *m_socket;
					}

					BoostSocket::BoostSocketValueType & BoostSocket::operator*( ) {
						assert( static_cast<bool>(m_socket) );
						return *m_socket;
					}

					BoostSocket::BoostSocketValueType * BoostSocket::operator->( ) const {
						assert( static_cast<bool>(m_socket) );
						return m_socket.operator->( );
					}

					BoostSocket::BoostSocketValueType * BoostSocket::operator->( ) {
						assert( static_cast<bool>(m_socket) );
						return m_socket.operator->( );
					}

					bool is_open( BoostSocket const & socket ) {
						return socket->lowest_layer( ).is_open( );
					}

					void shutdown( BoostSocket socket, boost::asio::ip::tcp::socket::shutdown_type ) {
						socket->shutdown( );
					}

					boost::system::error_code shutdown( BoostSocket socket, boost::asio::ip::tcp::socket::shutdown_type, boost::system::error_code & ec ) {
						return socket->shutdown( ec );
					}

					void close( BoostSocket socket ) {
						socket->shutdown( );
					}

					boost::system::error_code close( BoostSocket socket, boost::system::error_code & ec ) {
						return socket->shutdown( ec );
					}

					void cancel( BoostSocket socket ) {
						socket->lowest_layer( ).cancel( );
					}

					boost::asio::ip::tcp::endpoint remote_endpoint( BoostSocket const & socket ) {
						return socket->lowest_layer( ).remote_endpoint( );
					}

					boost::asio::ip::tcp::endpoint local_endpoint( BoostSocket const & socket ) {
						return socket->lowest_layer( ).local_endpoint( );
					}
				}	// namespace impl
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw