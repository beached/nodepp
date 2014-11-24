#pragma once

#include <cinttypes>
#include <functional>
#include <memory>
#include <string>

#include "lib_event_emitter.h"
#include "lib_net_server.h"
#include "lib_net_socket.h"
#include "lib_http_client_request.h"
#include "lib_http_headers.h"
#include "lib_http_chunk.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				class Handle;
			}

			namespace http {
				class Error { };
				static std::map<int16_t, std::string> const STATUS_CODES;

				using head_t = std::vector < uint8_t > ;

				struct HttpServerException: public std::runtime_error {
					uint32_t error_number;
					template<typename StringType>
					HttpServerException( const int32_t errorNumber, const StringType& msg ) : std::runtime_error( msg ), error_number( errorNumber ) { }
				};

				class Agent { 
				public:
					size_t& max_sockets( );
					size_t const & max_sockets( ) const;

					std::vector<daw::nodepp::lib::net::Socket> const & sockets( ) const;
					std::vector<ClientRequest> const & requests( ) const;
				};

				static Agent global_agent;

				enum class ServerResponseEvents { close, finish, newListener, removeListener };
				class ServerResponse: virtual public daw::nodepp::base::generic::EventEmitter<ServerResponseEvents> {
					ServerResponse( );
					friend class Server;
				public:
					ServerResponse& write_continue( );
					ServerResponse& write_head( uint16_t status_code, std::string reason_phrase = "", Headers headers = Headers{ } );
					
					template<typename Listener>
					ServerResponse& set_timeout( size_t msecs, Listener listener ) {
						throw std::runtime_error( "Method Not Implemented" );
					}
					
					uint16_t& status_code( );
					uint16_t const & status_code( ) const;

					void set_header( std::string name, std::string value );
					void set_header( Headers headers );

					bool headers_sent( ) const;

					bool& send_date( );
					bool const& send_date( ) const;

					Header const & get_header( std::string name ) const;
					ServerResponse& remove_header( std::string name );

					bool write_chunk( Chunk chunk, daw::nodepp::lib::encoding_t encoding = "" );
					bool add_trailers( Headers headers );

					void end( );
					void end( Chunk chunk, daw::nodepp::lib::encoding_t encoding = "" );
				};

				enum class ServerEvents { request, connection, close, checkContinue, connect, upgrade, clientError, listening, newListener, removeListener };
				class Server: virtual public daw::nodepp::base::generic::EventEmitter<ServerEvents> {
				public:
					Server( );

					Server( Server&& other );
					Server& operator=(Server&& rhs);
					Server( Server const & ) = default;
					Server& operator=(Server const &) = default;
					virtual ~Server( );

					template<typename Listener>
					Server& on( ServerEvents event, Listener& listener ) {
						add_listener( event, listener );
						return *this;
					}

					template<typename Listener>
					Server& once( ServerEvents event, Listener& listener ) {
						add_listener( event, listener, true );
						return *this;
					}

					Server& listen( uint16_t port, std::string hostname = "", uint16_t backlog = 511 );
					template<typename Listener>
					Server& listen( uint16_t port, std::string hostname, uint16_t backlog, Listener listener ) {
						return this->rollback_event_on_exception( ServerEvents::listening, listener, [&]( ) {
							return listen( port, hostname, backlog );
						} );
					}

					Server& listen( std::string path );
					template<typename Listener>
					Server& listen( std::string path, Listener listener ) {
						return this->rollback_event_on_exception( ServerEvents::listening, listener, [&]( ) {
							return listen( path );
						} );
					}

					Server& listen( daw::nodepp::lib::net::Handle& handle );
					template<typename Listener>
					Server& listen( daw::nodepp::lib::net::Handle& handle, Listener listener ) {
						return this->rollback_event_on_exception( ServerEvents::listening, listener, [&]( ) {
							return listen( handle );
						} );
					}


					size_t& max_header_count( );
					size_t const & max_header_count( ) const;

					template<typename Listener>
					Server& set_timeout( size_t msecs, Listener listener ) {
						throw std::runtime_error( "Method Not Implemented" );
					}

					size_t timeout( ) const;
				};	// class Server

				template<typename Listener>
				Server create_server( Listener listener ) {
					return Server( ).on( ServerEvents::listening, listener );
				}

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
