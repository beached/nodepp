#pragma once

#include <cinttypes>
#include <functional>
#include <memory>
#include <string>

#include "event_listener.h"
#include "plugin_base.h"


namespace daw {
	namespace nodepp {
		namespace plugins {
			// TODO START
			class HttpRequest { };
			class HttpResponse { };
			class Socket { };
			// END TODO
			
			struct HttpServerException: public std::runtime_error {
				uint32_t error_number;
				template<typename StringType>
				HttpServerException( const int32_t errorNumber, const StringType& msg ): std::runtime_error( msg ), error_number( errorNumber ) { }
			};

			class HttpServer: public IPlugin {
			public:				
				using head_t = std::vector < uint8_t > ;

				virtual std::string name( ) const override;
				virtual int64_t version( ) const override;

				enum class EventType: int32_t { request, connection, close, checkContinue, connect, upgrade, clientError };
				struct events {
					daw::nodepp::base::Event<HttpRequest, HttpResponse> request;
					daw::nodepp::base::Event<Socket> connection;
					daw::nodepp::base::Event<> close;
					daw::nodepp::base::Event<HttpRequest, HttpResponse> check_continue;
					daw::nodepp::base::Event<HttpRequest, Socket, head_t> connect;
					daw::nodepp::base::Event<HttpRequest, Socket, head_t> upgrade;
					daw::nodepp::base::Event<HttpServerException, Socket> client_error;
				};	// struct events
			};
		}	// namespace plugins
	}	// namespace nodepp
}	// namespace daw


std::unique_ptr<daw::nodepp::plugins::HttpServer> create_plugin();
