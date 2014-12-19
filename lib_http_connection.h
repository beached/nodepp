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

				class HttpConnection {
					std::shared_ptr<impl::HttpConnectionImpl> m_impl;

				public:
					HttpConnection( ) = default;
					HttpConnection( HttpConnection const & ) = default;
					HttpConnection( lib::net::NetSocketStream socket );
					HttpConnection( HttpConnection && );
					HttpConnection& operator=( HttpConnection );
					virtual ~HttpConnection( );

					std::vector<std::string> const & valid_events( ) const;

					// Event callbacks															
					HttpConnection& when_error( std::function<void( base::Error )> listener );
					HttpConnection& when_next_error( std::function<void( base::Error )> listener );

					HttpConnection& when_client_error( std::function<void( base::Error )> listener );
					HttpConnection& when_next_client_error( std::function<void( base::Error )> listener );

					HttpConnection& when_request_made( std::function<void( std::shared_ptr<HttpClientRequest>, HttpServerResponse )> listener );
					HttpConnection& when_next_request_made( std::function<void( std::shared_ptr<HttpClientRequest>, HttpServerResponse )> listener );

					HttpConnection& on_closed( std::function<void( )> listener );	// Only once as it is called on the way out				
					void close( );

					lib::net::NetSocketStream& socket( );
					lib::net::NetSocketStream const & socket( ) const;
				};	// class HttpConnection
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
