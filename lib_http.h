#pragma once

#include <cinttypes>
#include <functional>
#include <memory>
#include <string>

#include "event_listener.h"
#include "lib_net_server.h"


namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				// TODO START
				class HttpRequest { };
				class HttpResponse { };
				class Socket { };
				class Handle { };
				// END TODO

				struct HttpServerException: public std::runtime_error {
					uint32_t error_number;
					template<typename StringType>
					HttpServerException( const int32_t errorNumber, const StringType& msg ) : std::runtime_error( msg ), error_number( errorNumber ) { }
				};

				class Server {
				public:
					using namespace daw::nodepp;

					using head_t = std::vector < uint8_t > ;

					enum class EventType: int32_t { request, connection, close, checkContinue, connect, upgrade, clientError };
					struct events {
						using event_t_request = base::Event <HttpRequest, HttpResponse> ;
						event_t_request request;
						using request_callback_t = event_t_request::callback_t;

						using event_t_connection = base::Event <Socket> ;
						event_t_connection connection;
						using connection_callback_t = event_t_connection::callback_t;

						using event_t_close = base::Event <> ;
						event_t_close close;
						using close_callback_t = event_t_close::callback_t;

						using event_t_check_continue = base::Event < HttpRequest, HttpResponse > ;
						event_t_check_continue check_continue;
						using check_continue_callback_t = event_t_check_continue::callback_t;

						using event_t_connect = base::Event < HttpRequest, HttpResponse > ;
						event_t_connect connect;
						using connect_callback_t = event_t_connect::callback_t;

						using event_t_upgrade = base::Event < HttpRequest, HttpResponse > ;
						event_t_upgrade upgrade;
						using upgrade_callback_t = event_t_upgrade::callback_t;

						using event_t_client_error = base::Event < HttpRequest, HttpResponse > ;
						event_t_client_error client_error;
						using client_error_callback_t = event_t_client_error::callback_t;
					};	// struct events

					void max_header_count( size_t max_headers = 1000 );

				};
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
