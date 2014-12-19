#pragma once

#include <memory>

#include "lib_net_socket_stream.h"
#include "lib_http_request.h"
#include "lib_http_server_response.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				namespace impl { class HttpConnectionImpl; }

				using namespace daw::nodepp;

				enum class HttpConnectionState { Request, Message };

				class HttpConnection: public std::enable_shared_from_this<HttpConnection> {
					std::shared_ptr<impl::HttpConnectionImpl> m_impl;

				public:
					HttpConnection( ) = default;
					HttpConnection( HttpConnection const & ) = default;
					HttpConnection( lib::net::NetSocketStream socket );
					HttpConnection( HttpConnection && );
					HttpConnection& operator=( HttpConnection );
					virtual ~HttpConnection( ) = default;

					std::vector<std::string> const & valid_events( ) const;					

					// Event callbacks															
					virtual void when_listener_added( std::function<void( std::string, base::Callback )> listener );
					virtual void when_listener_removed( std::function<void( std::string, base::Callback )> listener );
					virtual void when_error( std::function<void( base::Error )> listener );

					virtual void when_next_listener_added( std::function<void( std::string, base::Callback )> listener );
					virtual void when_next_listener_removed( std::function<void( std::string, base::Callback )> listener );
					virtual void when_next_error( std::function<void( base::Error )> listener );

					virtual void when_client_error( std::function<void( base::Error )> listener );
					virtual void when_next_client_error( std::function<void( base::Error )> listener );

					virtual void when_request_made( std::function<void( std::shared_ptr<HttpClientRequest>, HttpServerResponse )> listener );
					virtual void when_next_request_made( std::function<void( std::shared_ptr<HttpClientRequest>, HttpServerResponse )> listener );

					void when_closed( std::function<void( )> listener );	// Only once as it is called on the way out				
					void close( );

					std::shared_ptr<HttpConnection> get_ptr( );

					lib::net::NetSocketStream& socket( );
					lib::net::NetSocketStream const & socket( ) const;
				};	// class HttpConnection
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
