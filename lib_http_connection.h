#pragma once

#include <memory>

#include "lib_net_socket.h"
#include "lib_http_request.h"
#include "lib_http_server_response.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;

				enum class HttpConnectionState { Request, Message };

				class HttpConnection: public base::EventEmitter {
					std::shared_ptr<lib::net::NetSocket> m_socket_ptr;

					void reset( );
				public:
					HttpConnection( ) = default;
					HttpConnection( std::shared_ptr<lib::net::NetSocket> socket_ptr );
					HttpConnection( HttpConnection const & ) = default;
					HttpConnection& operator=( HttpConnection const & ) = default;
					virtual ~HttpConnection( ) = default;
					virtual std::vector<std::string> const & valid_events( ) const override;

					// Event callbacks															
					virtual HttpConnection& on_error( std::function<void( base::Error )> listener ) override;
					virtual HttpConnection& once_error( std::function<void( base::Error )> listener ) override;

					virtual HttpConnection& on_clientError( std::function<void( base::Error )> listener );
					virtual HttpConnection& once_clientError( std::function<void( base::Error )> listener );

					virtual HttpConnection& on_requestGet( std::function<void( HttpClientRequest, HttpServerResponse )> listener );
					virtual HttpConnection& once_requestGet( std::function<void( HttpClientRequest, HttpServerResponse )> listener );

					virtual HttpConnection& on_requestPost( std::function<void( HttpClientRequest, HttpServerResponse )> listener );
					virtual HttpConnection& once_requestPost( std::function<void( HttpClientRequest, HttpServerResponse )> listener );

					virtual HttpConnection& on_requestPut( std::function<void( HttpClientRequest, HttpServerResponse )> listener );
					virtual HttpConnection& once_requestPut( std::function<void( HttpClientRequest, HttpServerResponse )> listener );

					virtual HttpConnection& on_requestHead( std::function<void( HttpClientRequest, HttpServerResponse )> listener );
					virtual HttpConnection& once_requestHead( std::function<void( HttpClientRequest, HttpServerResponse )> listener );

					virtual HttpConnection& on_requestDelete( std::function<void( HttpClientRequest, HttpServerResponse )> listener );
					virtual HttpConnection& once_requestDelete( std::function<void( HttpClientRequest, HttpServerResponse )> listener );

					virtual HttpConnection& on_requestConnect( std::function<void( HttpClientRequest, HttpServerResponse )> listener );
					virtual HttpConnection& once_requestConnect( std::function<void( HttpClientRequest, HttpServerResponse )> listener );

					virtual HttpConnection& on_requestOptions( std::function<void( HttpClientRequest, HttpServerResponse )> listener );
					virtual HttpConnection& once_requestOptions( std::function<void( HttpClientRequest, HttpServerResponse )> listener );

					virtual HttpConnection& on_requestTrace( std::function<void( HttpClientRequest, HttpServerResponse )> listener );
					virtual HttpConnection& once_requestTrace( std::function<void( HttpClientRequest, HttpServerResponse )> listener );

					virtual HttpConnection& on_request( std::function<void( HttpRequestMethod, HttpClientRequest, HttpServerResponse )> listener );
					virtual HttpConnection& once_request( std::function<void( HttpRequestMethod, HttpClientRequest, HttpServerResponse )> listener );

					virtual HttpConnection& once_close( std::function<void( )> listener );	// Only once as it is called on the way out				
					void close( );
				};	// class HttpConnection
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
