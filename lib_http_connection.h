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
				HttpConnection create_http_connection( lib::net::SharedNetSocketStream );
								
				namespace impl {
					class HttpConnectionImpl: public std::enable_shared_from_this<HttpConnectionImpl>, public base::StandardEvents<HttpConnectionImpl> {
						lib::net::SharedNetSocketStream m_socket;
						base::SharedEventEmitter m_emitter;

						HttpConnectionImpl( lib::net::SharedNetSocketStream socket, base::SharedEventEmitter emitter = base::create_shared_event_emitter( ) );
					public:
						friend HttpConnection lib::http::create_http_connection( lib::net::SharedNetSocketStream );

						HttpConnectionImpl( ) = delete;
						
						HttpConnectionImpl( HttpConnectionImpl const & ) = delete;
						HttpConnectionImpl& operator=(HttpConnectionImpl const &) = delete;
						HttpConnectionImpl( HttpConnectionImpl && ) = delete;
						HttpConnectionImpl& operator=(HttpConnectionImpl &&) = delete;
						
						~HttpConnectionImpl( ) = default;

						void on_client_error( std::function<void( base::Error )> listener );
						void on_next_client_error( std::function<void( base::Error )> listener );

						void on_request_made( std::function<void( std::shared_ptr<HttpClientRequest>, HttpServerResponse )> listener );
						void on_next_request_made( std::function<void( std::shared_ptr<HttpClientRequest>, HttpServerResponse )> listener );

						void on_closed( std::function<void( )> listener );	// Only once as it is called on the way out				
						void close( );

						lib::net::SharedNetSocketStream socket( );

						std::shared_ptr<HttpConnectionImpl> get_ptr( );
					protected:
						void emit_closed( );
						void emit_client_error( base::Error error );
						void emit_request_made( std::shared_ptr<HttpClientRequest> request, HttpServerResponse response );
					};	// class HttpConnectionImpl
				}	// namespace impl

				HttpConnection create_http_connection( lib::net::SharedNetSocketStream socket ) {
					return HttpConnection( new impl::HttpConnectionImpl( std::move( socket ) ) );
				}
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
