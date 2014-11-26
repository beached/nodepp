#pragma once

#include <cinttypes>
#include <functional>
#include <memory>
#include <string>

#include "base_event_emitter.h"
#include "lib_http_chunk.h"
#include "lib_http_client_request.h"
#include "lib_http_headers.h"
#include "lib_net_server.h"
#include "lib_net_socket.h"
#include "lib_http_server_response.h"

namespace daw {
	namespace nodepp {
		namespace lib {			
			namespace http {
				using namespace daw::nodepp;
				static std::map<int16_t, std::string> const STATUS_CODES;

				using head_t = std::vector < uint8_t > ;

				class Agent { 
				public:
					size_t& max_sockets( );
					size_t const & max_sockets( ) const;

					std::vector<lib::net::NetSocket> const & sockets( ) const;
					std::vector<HttpClientRequest> const & requests( ) const;
				};

				static Agent global_agent;

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
