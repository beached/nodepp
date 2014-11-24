#pragma once

#include <cinttypes>
#include <functional>
#include <memory>
#include <string>

#include "lib_event.h"
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
					struct events {
						template<typename... Args>
						using event_t = daw::nodepp::lib::EventEmitter < Args... > ;

						using event_t_close = event_t< > ;
						event_t_close close;
						using close_callback_t = event_t_close::callback_t;

						using event_t_finish = event_t< > ;
						event_t_finish finish;
						using finish_callback_t = event_t_finish::callback_t;
					};

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

				class ServerImpl;

				class Server {
					std::shared_ptr<ServerImpl> m_impl;
				public:
					struct events_t {
						CREATE_EVENT( request, Request, Response );
						CREATE_EVENT( check_continue, Request, Response );
						CREATE_EVENT( connection, daw::nodepp::lib::net::Socket );
						CREATE_EVENT( close );
						CREATE_EVENT( connect, Request, daw::nodepp::lib::net::Socket, head_t );
						CREATE_EVENT( upgrade, Request, daw::nodepp::lib::net::Socket, head_t );
						CREATE_EVENT( client_error, daw::nodepp::lib::http::Error, daw::nodepp::lib::net::Socket );
						
						using callback_t_listening = daw::nodepp::lib::net::Server::events_t::callback_t_listening;
						enum class types { request, connection, close, check_continue, connect, upgrade, client_error, listening };
						bool has_moved;
						events_t( ) = default;
						~events_t( ) = default;
						events_t( events_t const & ) = delete;
						events_t& operator=( events_t const & ) = delete;
						events_t( events_t && ) = delete;
						events_t& operator=( events_t && ) = delete;
					};	// struct events

					

					Server( );
					Server( events_t::callback_t_request func );
					Server( Server&& other );
					Server& operator=(Server&& rhs);
					Server( Server const & ) = default;
					Server& operator=(Server const &) = default;
					~Server( );


					Server& listen( uint16_t port, std::string hostname = "", uint16_t backlog = 511, events_t::callback_t_listening callback = events_t::callback_t_listening{ } );
					Server& listen( std::string path, events_t::callback_t_listening callback = events_t::callback_t_listening{ } );
					Server& listen( daw::nodepp::lib::net::Handle& handle, events_t::callback_t_listening callback = events_t::callback_t_listening{ } );

					Server& on( events_t::types et, events_t::callback_t_request callback );	// request, check_continue event(s)
					Server& once( events_t::types et, events_t::callback_t_request callback );	// request, check_continue event(s)

					Server& on( events_t::types et, events_t::callback_t_connection callback );	// connection event(s)
					Server& once( events_t::types et, events_t::callback_t_connection callback );	// connection event(s)

					Server& on( events_t::types et, events_t::callback_t_close callback );	// close event(s)
					Server& once( events_t::types et, events_t::callback_t_close callback );	// close event(s)

					Server& on( events_t::types et, events_t::callback_t_connect callback );	// connect, upgrade event(s)
					Server& once( events_t::types et, events_t::callback_t_connect callback );	// connect, upgrade event(s)

					Server& on( events_t::types et, events_t::callback_t_client_error callback );	// client_error event(s)
					Server& once( events_t::types et, events_t::callback_t_client_error callback );	// client_error event(s)

					size_t& max_header_count( );
					size_t const & max_header_count( ) const;

					Server& set_timeout( size_t msecs, daw::nodepp::lib::net::Socket::events_t::callback_t_timeout callback );
					size_t timeout( ) const;
				};	// class Server

				Server create_server( std::function<void( Request, Response )> func );

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
