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

				class HttpConnection: public base::EventEmitter {
					std::shared_ptr<lib::net::NetSocketStream> m_socket_ptr;

					
				public:
					void reset( );
					HttpConnection( ) = delete;
					HttpConnection( std::shared_ptr<lib::net::NetSocketStream> socket_ptr );
					HttpConnection( HttpConnection const & ) = default;
					HttpConnection& operator=( HttpConnection const & ) = default;
					virtual ~HttpConnection( ) = default;
					virtual std::vector<std::string> const & valid_events( ) const override;

					// Event callbacks															
					virtual HttpConnection& when_error( std::function<void( base::Error )> listener ) override;
					virtual HttpConnection& when_next_error( std::function<void( base::Error )> listener ) override;

					virtual HttpConnection& when_client_error( std::function<void( base::Error )> listener );
					virtual HttpConnection& when_next_client_error( std::function<void( base::Error )> listener );

					virtual HttpConnection& when_request_made( std::function<void( std::shared_ptr<HttpClientRequest>, std::shared_ptr<HttpServerResponse> )> listener );
					virtual HttpConnection& when_next_request_made( std::function<void( std::shared_ptr<HttpClientRequest>, std::shared_ptr<HttpServerResponse> )> listener );

					virtual HttpConnection& on_closed( std::function<void( )> listener );	// Only once as it is called on the way out				
					void close( );

					lib::net::NetSocketStream& socket( );
					lib::net::NetSocketStream const & socket( ) const;
				};	// class HttpConnection
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
