#pragma once
#include <boost/asio/ip/tcp.hpp>
#include <list>
#include <memory>
#include <string>

#include "base_error.h"
#include "base_event_emitter.h"
#include "base_types.h"
#include "lib_net_address.h"
#include "lib_net_socket.h"
#include "lib_net_socket_handle.h"
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
				class NetServer: public daw::nodepp::base::EventEmitter {
					std::shared_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;
					std::list<std::shared_ptr<NetSocket>> m_current_connections;

					void handle_accept( std::shared_ptr<NetSocket> socket, boost::system::error_code const & err );
					void start_accept( );					
				public:
					NetServer( );
					NetServer( NetServer const & ) = default;
					NetServer& operator=(NetServer const &) = default;
					NetServer( NetServer&& other );
					NetServer& operator=(NetServer&& rhs);
					virtual ~NetServer( );
					
					virtual std::vector<std::string> const & valid_events( ) const override;

					NetServer& listen( uint16_t port );
					NetServer& listen( uint16_t port, std::string hostname, uint16_t backlog = 511 );										
					NetServer& listen( std::string socket_path );					
					NetServer& listen( SocketHandle handle );				
					NetServer& close( );
					

					daw::nodepp::lib::net::NetAddress const& address( ) const;
					NetServer& unref( );
					NetServer& ref( );
					NetServer& set_max_connections( uint16_t value );

					NetServer& get_connections( std::function<void( daw::nodepp::base::Error err, uint16_t count )> callback );

					// Event callbacks
					
					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when an error occurs
					/// Inherited from EventEmitter
					virtual NetServer& on_error( std::function<void( base::Error )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when a connection is established
					NetServer& on_connection( std::function<void( std::shared_ptr<NetSocket> socket_ptr )> listener );					
					
					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the server is bound after calling 
					/// listen( ... )
					NetServer& on_listening( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the server closes and all connections 
					/// are closed
					NetServer& on_close( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when an error occurs
					/// Inherited from EventEmitter
					virtual NetServer& once_error( std::function<void( base::Error )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when a connection is established
					NetServer& once_connection( std::function<void( std::shared_ptr<NetSocket> socket_ptr )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the server is bound after calling 
					/// listen( ... )
					NetServer& once_listening( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the server closes and all connections 
					/// are closed
					NetServer& once_close( std::function<void( )> listener );

				};	// class server

			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
