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
					void handle_connection( std::shared_ptr<lib::net::NetSocketStream> socket_ptr );
					void handle_error( base::Error error );					
				public:
					HttpServer( );

					HttpServer( HttpServer&& other );
					HttpServer& operator=(HttpServer&& rhs);
					HttpServer( HttpServer const & ) = default;
					HttpServer& operator=(HttpServer const &) = default;
					virtual ~HttpServer( );					

					virtual std::vector<std::string> const & valid_events( ) const override;

					HttpServer& listen( uint16_t port );

					HttpServer& listen( uint16_t port, std::string hostname, uint16_t backlog = 511 );
					
					template<typename Listener>
					HttpServer& listen( uint16_t port, std::string hostname, uint16_t backlog, Listener listener ) {
						return base::rollback_event_on_exception( this, "listening", listener, [&]( ) {
							return listen( port, hostname, backlog );
						} );
					}

					HttpServer& listen( std::string path );
					
					template<typename Listener>
					HttpServer& listen( std::string path, Listener listener ) {
						return base::rollback_event_on_exception( this, "listening", listener, [&]( ) {
							return listen( path );
						} );
					}

					HttpServer& listen( base::ServiceHandle handle );
					
					template<typename Listener>
					HttpServer& listen( base::ServiceHandle handle, Listener listener ) {
						return base::rollback_event_on_exception( this, "listening", listener, [&]( ) {
							return listen( std::move( handle ) );
						} );
					}

					size_t& max_header_count( );
					size_t const & max_header_count( ) const;

					template<typename Listener>
					HttpServer& set_timeout( size_t msecs, Listener listener ) {
						throw std::runtime_error( "Method not implemented" );
					}					
					
					HttpServer& on_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener );
					HttpServer& once_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener );

					HttpServer& on_connection( std::function<void( std::shared_ptr<HttpConnection> )> listener );
					HttpServer& once_connection( std::function<void( std::shared_ptr<HttpConnection> )> listener );

					HttpServer& on_close( std::function<void( )> listener );
					HttpServer& once_close( std::function<void( )> listener );

					size_t timeout( ) const;
				};	// class Server
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
