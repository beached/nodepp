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
				namespace impl {
					class HttpServerImpl;
				}
				using HttpServer = std::shared_ptr < impl::HttpServerImpl > ;

				HttpServer create_http_server( base::EventEmitter emitter = base::create_event_emitter( ) );

				namespace impl {
					//////////////////////////////////////////////////////////////////////////
					// Summary:		An HTTP Server class 
					// Requires:	lib::net::NetServer
					class HttpServerImpl: public base::enable_shared<HttpServerImpl>, public base::StandardEvents < HttpServerImpl > {
						lib::net::NetServer m_netserver;
						base::EventEmitter m_emitter;
						std::list<HttpConnection> m_connections;

						static void handle_connection( std::weak_ptr<HttpServerImpl> obj, lib::net::NetSocketStream socket );

						HttpServerImpl( base::EventEmitter emitter );
					public:
						friend lib::http::HttpServer lib::http::create_http_server( base::EventEmitter );

						HttpServerImpl( HttpServerImpl&& other );
						HttpServerImpl& operator=(HttpServerImpl const &) = default;
						HttpServerImpl& operator=(HttpServerImpl && rhs);
						HttpServerImpl( HttpServerImpl const & ) = default;
						~HttpServerImpl( ) = default;

						base::EventEmitter& emitter( );

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

						HttpServerImpl& on_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener );
						HttpServerImpl& on_next_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener );

						HttpServerImpl& on_client_connected( std::function<void( HttpConnection )> listener );
						HttpServerImpl& on_next_client_connected( std::function<void( HttpConnection )> listener );

						HttpServerImpl& on_closed( std::function<void( )> listener );
						HttpServerImpl& on_next_closed( std::function<void( )> listener );

						size_t timeout( ) const;

						void emit_client_connected( HttpConnection connection );
						void emit_closed( );
						void emit_listening( boost::asio::ip::tcp::endpoint endpoint );
					};	// class Server
				}	// namespace impl
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
