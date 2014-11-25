#pragma once

#include "lib_net_server.h"
#include "base_event_emitter.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				class HttpServer: public daw::nodepp::lib::net::NetServer {
				protected:
					virtual bool event_is_valid( std::string const & event ) const override;
				public:
					HttpServer( );

					HttpServer( HttpServer&& other );
					HttpServer& operator=(HttpServer&& rhs);
					HttpServer( HttpServer const & ) = default;
					HttpServer& operator=(HttpServer const &) = default;
					virtual ~HttpServer( );					

					HttpServer& listen( uint16_t port, std::string hostname = "", uint16_t backlog = 511 );
					template<typename Listener>
					HttpServer& listen( uint16_t port, std::string hostname, uint16_t backlog, Listener listener ) {
						return this->rollback_event_on_exception( "listening", listener, [&]( ) {
							return listen( port, hostname, backlog );
						} );
					}

					HttpServer& listen( std::string path );
					template<typename Listener>
					HttpServer& listen( std::string path, Listener listener ) {
						return this->rollback_event_on_exception( "listening", listener, [&]( ) {
							return listen( path );
						} );
					}

					HttpServer& listen( daw::nodepp::lib::net::Handle& handle );
					template<typename Listener>
					HttpServer& listen( daw::nodepp::lib::net::Handle& handle, Listener listener ) {
						return this->rollback_event_on_exception( "listening", listener, [&]( ) {
							return listen( handle );
						} );
					}

// 					template<typename Listener>
// 					Server& on( std::string event, Listener listen )

					size_t& max_header_count( );
					size_t const & max_header_count( ) const;

					template<typename Listener>
					HttpServer& set_timeout( size_t msecs, Listener listener ) {
						throw std::runtime_error( "Method Not Implemented" );
					}

					template<typename Listener>
					HttpServer& on( std::string event, Listener& listener ) {
						add_listener( event, listener );
						return *this;
					}

					template<typename Listener>
					HttpServer& once( std::string event, Listener& listener ) {
						add_listener( event, listener, true );
						return *this;
					}

					size_t timeout( ) const;
				};	// class Server

				template<typename Listener>
				HttpServer create_server( Listener listener ) {
					return HttpServer( ).on( "listening", listener );
				}
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
