#pragma once

#include "lib_net_server.h"
#include "base_event_emitter.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				class Server: public daw::nodepp::lib::net::Server {
				protected:
					virtual bool event_is_valid( std::string const & event ) const override;
				public:
					Server( );

					Server( Server&& other );
					Server& operator=(Server&& rhs);
					Server( Server const & ) = default;
					Server& operator=(Server const &) = default;
					virtual ~Server( );					

					Server& listen( uint16_t port, std::string hostname = "", uint16_t backlog = 511 );
					template<typename Listener>
					Server& listen( uint16_t port, std::string hostname, uint16_t backlog, Listener listener ) {
						return this->rollback_event_on_exception( "listening", listener, [&]( ) {
							return listen( port, hostname, backlog );
						} );
					}

					Server& listen( std::string path );
					template<typename Listener>
					Server& listen( std::string path, Listener listener ) {
						return this->rollback_event_on_exception( "listening", listener, [&]( ) {
							return listen( path );
						} );
					}

					Server& listen( daw::nodepp::lib::net::Handle& handle );
					template<typename Listener>
					Server& listen( daw::nodepp::lib::net::Handle& handle, Listener listener ) {
						return this->rollback_event_on_exception( "listening", listener, [&]( ) {
							return listen( handle );
						} );
					}

// 					template<typename Listener>
// 					Server& on( std::string event, Listener listen )

					size_t& max_header_count( );
					size_t const & max_header_count( ) const;

					template<typename Listener>
					Server& set_timeout( size_t msecs, Listener listener ) {
						throw std::runtime_error( "Method Not Implemented" );
					}

					template<typename Listener>
					Server& on( std::string event, Listener& listener ) {
						add_listener( event, listener );
						return *this;
					}

					template<typename Listener>
					Server& once( std::string event, Listener& listener ) {
						add_listener( event, listener, true );
						return *this;
					}

					size_t timeout( ) const;
				};	// class Server

				template<typename Listener>
				Server create_server( Listener listener ) {
					return Server( ).on( "listening", listener );
				}
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
