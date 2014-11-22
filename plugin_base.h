#pragma once

#include <cinttypes>
#include <string>

namespace daw {
	namespace nodepp {
		namespace plugins {
			struct IPlugin {
				virtual std::string name( ) const = 0;
				virtual int64_t version( ) const = 0;
			};	// struct IPlugin
		}	// namespace plugins
	}	// namespace nodepp
}	// namespace daw
