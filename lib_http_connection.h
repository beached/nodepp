#pragma once

#include <memory>

#include "lib_net_socket.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;

				enum class HttpConnectionState { Request, Message };

				class HttpConnection: public base::EventEmitter {
					std::shared_ptr<lib::net::NetSocket> m_socket_ptr;
					HttpConnectionState m_state;

					void reset( );
				public:
					HttpConnection( std::shared_ptr<lib::net::NetSocket> socket_ptr );
					virtual ~HttpConnection( ) = default;
					virtual std::vector<std::string> const & valid_events( ) const override;

					// Event callbacks
					virtual HttpConnection& on_newListener( std::function<void( std::string, base::Callback )> listener ) override;
					virtual HttpConnection& on_removeListener( std::function<void( std::string, base::Callback )> listener ) override;
					virtual HttpConnection& on_error( std::function<void( base::Error )> listener ) override;

					virtual HttpConnection& once_newListener( std::function<void( std::string, base::Callback )> listener ) override;
					virtual HttpConnection& once_removeListener( std::function<void( std::string, base::Callback )> listener ) override;
					virtual HttpConnection& once_error( std::function<void( base::Error )> listener ) override;

					HttpConnection& once_close( std::function<void( )> listener );					
					void close( );
				};	// class HttpConnection
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
