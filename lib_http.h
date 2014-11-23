#pragma once

#include <cinttypes>
#include <functional>
#include <memory>
#include <string>

#include "event_listener.h"
#include "lib_net_server.h"
#include "lib_net_socket.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			class Error;
			namespace net {
				class Handle;
			}
			namespace http {				
				using head_t = std::vector < uint8_t > ;

				struct HttpServerException: public std::runtime_error {
					uint32_t error_number;
					template<typename StringType>
					HttpServerException( const int32_t errorNumber, const StringType& msg ) : std::runtime_error( msg ), error_number( errorNumber ) { }
				};

// 				class Server { };
				class IncomingMessage { };

				class ClientRequest {
				public:
					struct events {
						template<typename... Args>
						using event_t = daw::nodepp::base::Event < Args... > ;

						using event_t_response = event_t<IncomingMessage> ;
						event_t_response response;
						using response_callback_t = event_t_response::callback_t;

						using event_t_socket = event_t<daw::nodepp::lib::net::Socket> ;
						event_t_socket socket;
						using socket_callback_t = event_t_socket::callback_t;

						using event_t_connect = base::Event < IncomingMessage, daw::nodepp::lib::net::Socket, head_t> ;
						event_t_connect connect;
						using connect_callback_t = event_t_connect::callback_t;

						using event_t_upgrade = base::Event < IncomingMessage, daw::nodepp::lib::net::Socket, head_t > ;
						event_t_upgrade upgrade;
						using upgrade_callback_t = event_t_upgrade::callback_t;

						using event_t_continued = base::Event<> ;
						event_t_continued continued;
						using continued_callback_t = event_t_continued::callback_t;


					};
				
					bool write( std::string chunk, daw::nodepp::lib::encoding_t encoding = "" );
					bool write( daw::nodepp::lib::net::Socket::data_t chunk );
					
					void end( );
					void end( std::string data, daw::nodepp::lib::encoding_t encoding = "" );
					void end( daw::nodepp::lib::net::Socket::data_t data );

					void abort( );
						

				};

				class Agent { 
				public:
					size_t& max_sockets( );
					size_t const & max_sockets( ) const;

					std::vector<daw::nodepp::lib::net::Socket> const & sockets( ) const;
					std::vector<ClientRequest> const & requests( ) const;
				};

				static Agent global_agent;

				class ServerResponse {
					ServerResponse( );
					friend class Server;
				public:
					struct events {
						template<typename... Args>
						using event_t = daw::nodepp::base::Event < Args... > ;

						using event_t_close = event_t< > ;
						event_t_close close;
						using close_callback_t = event_t_close::callback_t;

						using event_t_finish = event_t< > ;
						event_t_finish finish;
						using finish_callback_t = event_t_finish::callback_t;
					};

					void write_continue( );
					void write_head( uint16_t status_code, std::string reason_phrase = "" );
					void write_head( uint16_t status_code, std::string reason_phrase, header_t headers );
					void set_timeout( size_t msecs, daw::nodepp::lib::net::Socket::events::timeout_callback_t callback );
					void set_status_code( uint16_t status_code );
					uint16_t status_code( ) const;
					using header_t = std::map < std::string, std::vector< std::string > > ;
					void set_header( std::string name, std::string value );
					void set_header( std::string name, std::vector<std::string> values );
					void set_header( header_t headers );
					bool headers_sent( ) const;
					void send_date( bool flag = true );
					std::vector<std::string> get_header( std::string name );
					void remove_header( std::string name );
					bool write_chunk( std::string chunk, daw::nodepp::lib::encoding_t encoding = "" );
					bool write_chunk( daw::nodepp::lib::net::Socket::data_t chunk );
					bool add_trailers( header_t headers );

					void end( );
					void end( std::string chunk, daw::nodepp::lib::encoding_t encoding = "" );
					void end( daw::nodepp::lib::net::Socket::data_t chunk );
					
					ClientRequest request( daw::nodepp::lib::options_t options, ClientRequest::events::response_callback_t callback );
					ClientRequest get( daw::nodepp::lib::options_t options, ClientRequest::events::response_callback_t callback );

				};

				class Server {
				private:
					class header_t;
					

					class HttpRequest { };
					

				public:

					struct events {
						template<typename... Args>
						using event_t = daw::nodepp::base::Event< Args... > ;

						using event_t_request = event_t<HttpRequest, IncomingMessage> ;
						event_t_request request;
						using request_callback_t = event_t_request::callback_t;

						using event_t_connection = event_t<daw::nodepp::lib::net::Socket> ;
						event_t_connection connection;
						using connection_callback_t = event_t_connection::callback_t;

						using event_t_close = event_t<> ;
						event_t_close close;
						using close_callback_t = event_t_close::callback_t;

						using event_t_check_continue = event_t< HttpRequest, ServerResponse > ;
						event_t_check_continue check_continue;
						using check_continue_callback_t = event_t_check_continue::callback_t;

						using event_t_connect = event_t< HttpRequest, daw::nodepp::lib::net::Socket,  head_t > ;
						event_t_connect connect;
						using connect_callback_t = event_t_connect::callback_t;

						using event_t_upgrade = event_t< HttpRequest, daw::nodepp::lib::net::Socket, head_t >;
						event_t_upgrade upgrade;
						using upgrade_callback_t = event_t_upgrade::callback_t;

						using event_t_client_error = event_t< daw::nodepp::lib::Error, daw::nodepp::lib::net::Socket > ;
						event_t_client_error client_error;
						using client_error_callback_t = event_t_client_error::callback_t;
					};	// struct events

					using listening_callback_t = daw::nodepp::lib::net::Server::events::listening_callback_t;
					void listen( uint16_t port, std::string hostname = "", uint16_t backlog = 511, listening_callback_t callback = nullptr );
					void listen( std::string path, listening_callback_t callback = nullptr );
					void listen( daw::nodepp::lib::net::Handle& handle, listening_callback_t callback = nullptr );

					void close( daw::nodepp::lib::net::Server::events::close_callback_t callback = nullptr );

					size_t& max_header_count( );
					size_t const & max_header_count( ) const;

					void set_timeout( size_t msecs, daw::nodepp::lib::net::Socket::events::timeout_callback_t callback );
					size_t timeout( ) const;
				};
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
