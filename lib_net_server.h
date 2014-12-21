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
				using namespace daw::nodepp;
				//////////////////////////////////////////////////////////////////////////
				// Summary:		A TCP Server class
				// Requires:	base::EventEmitter, base::options_t,
				//				lib::net::NetAddress, base::Error
				struct NetServer: public std::enable_shared_from_this<NetServer>, public base::StandardEvents<NetServer> {
					NetServer( std::shared_ptr<base::EventEmitter> emitter = std::make_shared<base::EventEmitter>( ) );
					NetServer( NetServer const & ) = default;
					NetServer( NetServer&& other );
					NetServer& operator=(NetServer && rhs);
					NetServer& operator=(NetServer const &) = default;
					~NetServer( ) = default;
					
					std::shared_ptr<NetServer> get_ptr( );

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
					void on_connection( std::function<void( SharedNetSocketStream socket )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when a connection is established
					void on_next_connection( std::function<void( SharedNetSocketStream socket )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when the server is bound after calling 
					/// listen( ... )
					void on_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when the server is bound after calling 
					/// listen( ... )
					void on_next_listening( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when the server closes and all connections 
					/// are closed
					void on_closed( std::function<void( )> listener );

				private:
					std::shared_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;
					std::shared_ptr<base::EventEmitter> m_emitter;

					static void handle_accept( std::weak_ptr<NetServer> obj, std::shared_ptr<NetSocketStream> socket, boost::system::error_code const & err );
					
					void start_accept( );

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when a connection is established
					void emit_connection( SharedNetSocketStream socket );

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when the server is bound after calling 
					///				listen( ... )
					void emit_listening( boost::asio::ip::tcp::endpoint endpoint );

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when the server is bound after calling 
					///				listen( ... )
					void emit_closed( );
				};	// class server

			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
