//	The MIT License (MIT)
//	
//	Copyright (c) 2014-2015 Darrell Wright
//	
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//	
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//	
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
//
#pragma once
#include <boost/asio/ip/tcp.hpp>
#include <list>
#include <memory>
#include <string>

#include "base_error.h"
#include "base_event_emitter.h"
#include "base_types.h"
#include "lib_net_address.h"
#include "lib_net_socket_stream.h"
#include "base_service_handle.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				namespace impl {
					class NetServerImpl;
				}

				using NetServer = std::shared_ptr < impl::NetServerImpl > ;

				NetServer create_net_server( base::EventEmitter emitter = base::create_event_emitter( ) );

				namespace impl {
					using namespace daw::nodepp;
					//////////////////////////////////////////////////////////////////////////
					// Summary:		A TCP Server class
					// Requires:	base::EventEmitter, base::options_t,
					//				lib::net::NetAddress, base::Error
					class NetServerImpl: public base::enable_shared<NetServerImpl>, public base::StandardEvents < NetServerImpl > {
						std::shared_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;
						base::EventEmitter m_emitter;
						NetServerImpl( base::EventEmitter emitter );
					public:
						friend lib::net::NetServer lib::net::create_net_server( base::EventEmitter );

						NetServerImpl( NetServerImpl const & ) = default;
						NetServerImpl( NetServerImpl&& other );
						NetServerImpl& operator=(NetServerImpl && rhs);
						NetServerImpl& operator=(NetServerImpl const &) = default;
						~NetServerImpl( ) = default;

						base::EventEmitter& emitter( );

						void listen( uint16_t port );
						void listen( uint16_t port, std::string hostname, uint16_t backlog = 511 );
						void listen( std::string socket_path );
						void close( );

						daw::nodepp::lib::net::NetAddress const& address( ) const;

						void set_max_connections( uint16_t value );

						void get_connections( std::function<void( daw::nodepp::base::Error err, uint16_t count )> callback );

						// Event callbacks

						//////////////////////////////////////////////////////////////////////////
						/// Summary:	Event emitted when a connection is established
						NetServerImpl& on_connection( std::function<void( NetSocketStream socket )> listener );

						//////////////////////////////////////////////////////////////////////////
						/// Summary:	Event emitted when a connection is established
						NetServerImpl& on_next_connection( std::function<void( NetSocketStream socket )> listener );

						//////////////////////////////////////////////////////////////////////////
						/// Summary:	Event emitted when the server is bound after calling 
						/// listen( ... )
						NetServerImpl& on_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener );

						//////////////////////////////////////////////////////////////////////////
						/// Summary:	Event emitted when the server is bound after calling 
						/// listen( ... )
						NetServerImpl& on_next_listening( std::function<void( )> listener );

						//////////////////////////////////////////////////////////////////////////
						/// Summary:	Event emitted when the server closes and all connections 
						/// are closed
						NetServerImpl& on_closed( std::function<void( )> listener );

						//////////////////////////////////////////////////////////////////////////
						/// Summary:	Event emitted when a connection is established
						void emit_connection( NetSocketStream socket );

						//////////////////////////////////////////////////////////////////////////
						/// Summary:	Event emitted when the server is bound after calling 
						///				listen( ... )
						void emit_listening( boost::asio::ip::tcp::endpoint endpoint );

						//////////////////////////////////////////////////////////////////////////
						/// Summary:	Event emitted when the server is bound after calling 
						///				listen( ... )
						void emit_closed( );

					private:

						static void handle_accept( std::weak_ptr<NetServerImpl> obj, NetSocketStream&& socket, boost::system::error_code const & err );

						void start_accept( );

					};	// class NetServerImpl
				}	// namespace impl

			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
