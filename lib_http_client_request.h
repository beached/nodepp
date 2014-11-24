#pragma once

#include <string>

#include "lib_event_emitter.h"


namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				class Request { };
				class IncomingMessage { };

				enum class ClientRequestEvents { response, socket, connect, upgrade, continued, newListener, removeListener };
				class ClientRequest: virtual public daw::nodepp::base::generic::EventEmitter<ClientRequestEvents> {
				public:

					bool write( std::string chunk, daw::nodepp::lib::encoding_t encoding = "" );
					bool write( daw::nodepp::lib::net::Socket::data_t chunk );

					void end( );
					void end( std::string data, daw::nodepp::lib::encoding_t encoding = "" );
					void end( daw::nodepp::lib::net::Socket::data_t data );
					void abort( );
				};

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
