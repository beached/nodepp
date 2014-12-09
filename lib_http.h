#pragma once

#include <cinttypes>
#include <functional>
#include <memory>
#include <string>

#include "base_event_emitter.h"
#include "lib_http_chunk.h"
#include "lib_http_headers.h"
#include "lib_net_server.h"
#include "lib_net_socket.h"
#include "lib_http_server_response.h"

namespace daw {
	namespace nodepp {
		namespace lib {			
			namespace http {
				using namespace daw::nodepp;
				std::map<uint16_t, std::string> const & status_codes( );

				using head_t = std::vector < uint8_t > ;

				
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
