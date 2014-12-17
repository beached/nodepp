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
					void handle_connection( lib::net::NetSocketStream socket );
					void handle_error( base::Error error );					
				public:
					HttpServer( );

					HttpServer( HttpServer&& other );
					HttpServer& operator=(HttpServer&& rhs);
					HttpServer( HttpServer const & ) = default;
					HttpServer& operator=(HttpServer const &) = default;
					virtual ~HttpServer( );					

					virtual std::vector<std::string> const & valid_events( ) const override;

					HttpServer& listen_on( uint16_t port );

					HttpServer& listen_on( uint16_t port, std::string hostname, uint16_t backlog = 511 );
					
					template<typename Listener>
					HttpServer& listen_on( uint16_t port, std::string hostname, uint16_t backlog, Listener listener ) {
						return base::rollback_event_on_exception( this, "listening", listener, [&]( ) {
							return listen_on( port, hostname, backlog );
						} );
					}

					HttpServer& listen_on( std::string path );
					
					template<typename Listener>
					HttpServer& listen_on( std::string path, Listener listener ) {
						return base::rollback_event_on_exception( this, "listening", listener, [&]( ) {
							return listen_on( path );
						} );
					}

					HttpServer& listen_on( base::ServiceHandle handle );
					
					template<typename Listener>
					HttpServer& listen_on( base::ServiceHandle handle, Listener listener ) {
						return base::rollback_event_on_exception( this, "listening", listener, [&]( ) {
							return listen_on( std::move( handle ) );
						} );
					}

					size_t& max_header_count( );
					size_t const & max_header_count( ) const;

					template<typename Listener>
					HttpServer& set_timeout( size_t msecs, Listener listener ) {
						throw std::runtime_error( "Method not implemented" );
					}					
					
					HttpServer& when_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener );
					HttpServer& when_next_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener );

					HttpServer& when_client_connected( std::function<void( std::shared_ptr<HttpConnection> )> listener );
					HttpServer& when_next_client_connected( std::function<void( std::shared_ptr<HttpConnection> )> listener );

					HttpServer& when_closed( std::function<void( )> listener );
					HttpServer& when_next_close( std::function<void( )> listener );

					size_t timeout( ) const;
				};	// class Server
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
