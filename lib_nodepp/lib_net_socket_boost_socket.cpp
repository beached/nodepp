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

#include "lib_net_socket_boost_socket.h"
#include "base_service_handle.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				namespace impl {
					void BoostSocket::init( ) {
						if( !m_socket ) {
							assert( static_cast<bool>( m_encryption_context ) );
							m_socket = std::make_shared<BoostSocketValueType>( base::ServiceHandle::get( ), *m_encryption_context );
							assert( static_cast<bool>( m_socket ) );
						}
					}

					void BoostSocket::reset_socket( ) {
						m_socket.reset( );
					}

					EncryptionContext & BoostSocket::encryption_context( ) {
							return *m_encryption_context;
					}

					EncryptionContext const & BoostSocket::encryption_context( ) const {
						return *m_encryption_context;
					}

					BoostSocket::BoostSocketValueType & BoostSocket::raw_socket( ) {
						init( );
						return *m_socket;
					}

					BoostSocket::BoostSocketValueType const & BoostSocket::raw_socket( ) const {
						assert( static_cast<bool>( m_socket ) );
						return *m_socket;
					}

					BoostSocket::operator bool( ) const {
						return static_cast<bool>(m_socket);
					}

					BoostSocket::BoostSocket( std::shared_ptr<EncryptionContext> context ): m_encryption_context( std::move( context ) ), m_encryption_enabled( false ), m_socket( nullptr ) { }

					BoostSocket::BoostSocket( std::shared_ptr<BoostSocket::BoostSocketValueType> socket, std::shared_ptr<EncryptionContext> context ): m_encryption_context( std::move( context ) ), m_encryption_enabled( false ), m_socket( std::move( socket ) ) { }

					BoostSocket::BoostSocketValueType const & BoostSocket::operator*( ) const {
						return raw_socket( );
					}

					BoostSocket::BoostSocketValueType & BoostSocket::operator*( ) {
						return raw_socket( );
					}

					BoostSocket::BoostSocketValueType * BoostSocket::operator->( ) const {
						assert( static_cast<bool>( m_socket ) );
						return m_socket.operator->( );
					}

					BoostSocket::BoostSocketValueType * BoostSocket::operator->( ) {
						init( );
						return m_socket.operator->( );
					}

					bool BoostSocket::encyption_on( ) const {
						return m_encryption_enabled;
					}

					bool & BoostSocket::encyption_on( ) {
						return m_encryption_enabled;
					}

					void BoostSocket::encyption_on( bool value ) {
						m_encryption_enabled = value;
					}

					bool BoostSocket::is_open( ) const {
						return raw_socket( ).next_layer( ).is_open( );
					}

					void BoostSocket::shutdown( boost::asio::ip::tcp::socket::shutdown_type ) {
						raw_socket( ).shutdown( );
					}

					boost::system::error_code BoostSocket::shutdown( boost::asio::ip::tcp::socket::shutdown_type, boost::system::error_code & ec ) {
						return raw_socket( ).shutdown( ec );
					}

					void BoostSocket::close( ) {
						raw_socket( ).shutdown( );
					}

					boost::system::error_code BoostSocket::close( boost::system::error_code & ec ) {
						return raw_socket( ).shutdown( ec );
					}

					void BoostSocket::cancel( ) {
						raw_socket( ).next_layer( ).cancel( );
					}

					boost::asio::ip::tcp::endpoint BoostSocket::remote_endpoint( ) const {
						return raw_socket( ).next_layer( ).remote_endpoint( );
					}

					boost::asio::ip::tcp::endpoint BoostSocket::local_endpoint( ) const {
						return raw_socket( ).next_layer( ).local_endpoint( );
					}
				}	// namespace impl
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw

