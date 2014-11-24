#pragma once

#include <string>
#include <vector>

#include "lib_event_emitter.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				struct Header {
					std::string name;
					std::string value;

					Header( );
					std::string to_string( ) const;
				};
				struct Headers {
					std::vector<Header> headers;

					Headers( );
					std::string to_string( ) const;
				};
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
