#pragma once

#include "event_listener.h"
#include "lib_net_handle.h"
#include "lib_types.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {				
				class Error;
				class Address;

				class Server: public Handle {
					using namespacec daw::nodepp;
				public:
					struct events {
						using event_t_listening = base::Event < > ;
						event_t_listening listening;
						using listening_callback_t = event_t_listening::callback_t;

						using event_t_connecton = base::Event < > ;
						event_t_connecton connection;
						using connection_callback_t = event_t_connecton::callback_t;

						using event_t_close = base::Event < > ;
						event_t_close close;
						using close_callback_t = event_t_close::callback_t;

						using event_t_error = base::Event < > ;
						event_t_error error;
						using error_callback_t = event_t_error::callback_t;
					};


					void listen( uint16_t port, std::string hostname = "", uint16_t backlog = 511, events::listening_callback_t callback = nullptr );
					void listen( std::string socket_path, events::listening_callback_t callback = nullptr );
					void listen( Handle const & handle, events::listening_callback_t = nullptr );
					void close( events::close_callback_t callback = nullptr );
					
					const Address& address( ) const;
					void unref( );
					void ref( );
					void max_connections( uint16_t value );

					void get_connections( std::function<void( Error err, uint16_t count )> callback );

				};	// class server
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
