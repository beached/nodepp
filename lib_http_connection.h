#pragma once

#include <memory>

#include "lib_net_socket_stream.h"
#include "lib_http_request.h"
#include "lib_http_server_response.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				enum class HttpConnectionState { Request, Message };

				namespace impl { class HttpConnectionImpl; }
				using HttpConnection = std::shared_ptr < impl::HttpConnectionImpl > ;
				HttpConnection create_http_connection( lib::net::NetSocketStream&& socket, base::EventEmitter emitter = base::create_event_emitter( ) );
								
				namespace impl {
					class HttpConnectionImpl: public base::enable_shared<HttpConnectionImpl>, public base::StandardEvents<HttpConnectionImpl> {
						lib::net::NetSocketStream m_socket;
						base::EventEmitter m_emitter;

						HttpConnectionImpl( lib::net::NetSocketStream && socket, base::EventEmitter emitter );
					public:
						friend HttpConnection lib::http::create_http_connection( lib::net::NetSocketStream&&, base::EventEmitter emitter );

						HttpConnectionImpl( ) = delete;
						
						HttpConnectionImpl( HttpConnectionImpl const & ) = delete;
						HttpConnectionImpl& operator=(HttpConnectionImpl const &) = delete;
						HttpConnectionImpl( HttpConnectionImpl && ) = delete;
						HttpConnectionImpl& operator=(HttpConnectionImpl &&) = delete;
						
						~HttpConnectionImpl( ) = default;

						base::EventEmitter& emitter( );

						HttpConnectionImpl& on_client_error( std::function<void( base::Error )> listener );
						HttpConnectionImpl& on_next_client_error( std::function<void( base::Error )> listener );

						HttpConnectionImpl& on_request_made( std::function<void( std::shared_ptr<HttpClientRequest>, HttpServerResponse )> listener );
						HttpConnectionImpl& on_next_request_made( std::function<void( std::shared_ptr<HttpClientRequest>, HttpServerResponse )> listener );

						HttpConnectionImpl& on_closed( std::function<void( )> listener );	// Only once as it is called on the way out				
						void close( );
						void start( );
						lib::net::NetSocketStream socket( );

						void emit_closed( );
						void emit_client_error( base::Error error );
						void emit_request_made( std::shared_ptr<HttpClientRequest> request, HttpServerResponse response );
					};	// class HttpConnectionImpl
				}	// namespace impl

			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
