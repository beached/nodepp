#pragma once

#include <cstdint>
#include <string>
#include <utility>

namespace daw {
	namespace nodepp {
		namespace lib {			
			namespace http {
				using namespace daw::nodepp;
				using http_status_code_t = std::pair<uint16_t, std::string>;
				std::pair<uint16_t, std::string> const & HttpStatusCodes( uint16_t code );				
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
