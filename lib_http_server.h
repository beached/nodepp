#pragma once

#include <list>
#include <memory>
#include <vector>

#include "lib_http_connection.h"
#include "lib_http_server_response.h"
#include "lib_net_server.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				//////////////////////////////////////////////////////////////////////////
				// Summary:		An HTTP Server class 
				// Requires:	lib::net::NetServer
				class HttpServer: public base::EventEmitter {
					lib::net::NetServer m_netserver;
					std::list<std::pair<bool,HttpConnection>> m_connections;
					void handle_connection( lib::net::NetSocketStream socket );
					void handle_error( base::Error error );					
				public:
					HttpServer( );

					HttpServer( HttpServer&& other );
					HttpServer& operator=(HttpServer rhs);
					HttpServer( HttpServer const & ) = default;
					virtual ~HttpServer( ) = default;

					virtual std::vector<std::string> const & valid_events( ) const override;

					void listen_on( uint16_t port );

					void listen_on( uint16_t port, std::string hostname, uint16_t backlog = 511 );
					
					template<typename Listener>
					void listen_on( uint16_t port, std::string hostname, uint16_t backlog, Listener listener ) {
						base::rollback_event_on_exception( this, "listening", listener, [&]( ) {
							listen_on( port, hostname, backlog );
						} );
					}

					void listen_on( std::string path );
					
					template<typename Listener>
					void listen_on( std::string path, Listener listener ) {
						base::rollback_event_on_exception( this, "listening", listener, [&]( ) {
							listen_on( path );
						} );
					}

					void listen_on( base::ServiceHandle handle );
					
					template<typename Listener>
					void listen_on( base::ServiceHandle handle, Listener listener ) {
						base::rollback_event_on_exception( this, "listening", listener, [&]( ) {
							listen_on( std::move( handle ) );
						} );
					}

					size_t& max_header_count( );
					size_t const & max_header_count( ) const;

					template<typename Listener>
					void set_timeout( size_t msecs, Listener listener ) {
						throw std::runtime_error( "Method not implemented" );
					}					
					
					void when_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener );
					void when_next_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener );

					void when_client_connected( std::function<void( HttpConnection )> listener );
					void when_next_client_connected( std::function<void( HttpConnection )> listener );

					void when_closed( std::function<void( )> listener );
					void when_next_close( std::function<void( )> listener );

					size_t timeout( ) const;

				protected:
					virtual void emit_connection( HttpConnection connection );
					virtual void emit_close( );
					virtual void emit_listening( boost::asio::ip::tcp::endpoint endpoint );
				};	// class Server
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
