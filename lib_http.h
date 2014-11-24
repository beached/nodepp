#pragma once

#include <cinttypes>
#include <functional>
#include <memory>
#include <string>

#include "lib_event_emitter.h"
#include "lib_net_server.h"
#include "lib_net_socket.h"
#include "lib_http_client_request.h"
	
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

				class Response {
					Response( );
					friend class Server;
				public:
					using Header = std::map < std::string, std::vector< std::string > >;
					

					void write_continue( );
					void write_head( uint16_t status_code, std::string reason_phrase = "" );
					void write_head( uint16_t status_code, std::string reason_phrase, Header headers );
					void set_timeout( size_t msecs, daw::nodepp::lib::net::Socket::events_t::callback_t_timeout callback );
					void set_status_code( uint16_t status_code );
					uint16_t status_code( ) const;
					
					void set_header( std::string name, std::string value );
					void set_header( std::string name, std::vector<std::string> values );
					void set_header( Header headers );
					bool headers_sent( ) const;
					void send_date( bool flag = true );
					std::vector<std::string> get_header( std::string name );
					void remove_header( std::string name );
					bool write_chunk( std::string chunk, daw::nodepp::lib::encoding_t encoding = "" );
					bool write_chunk( daw::nodepp::lib::net::Socket::data_t chunk );
					bool add_trailers( Header headers );

					void end( );
					void end( std::string chunk, daw::nodepp::lib::encoding_t encoding = "" );
					void end( daw::nodepp::lib::net::Socket::data_t chunk );
					
					ClientRequest request( daw::nodepp::lib::options_t options, ClientRequest::events_t::callback_t_response callback );
					ClientRequest get( daw::nodepp::lib::options_t options, ClientRequest::events_t::callback_t_response callback );

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


					Server& listen( uint16_t port, std::string hostname = "", uint16_t backlog = 511 );
					template<typename Listener>
					Server& listen( uint16_t port, std::string hostname = "", uint16_t backlog = 511, Listener listener ) {
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

				Server create_server( std::function<void( Request, Response )> func );

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
