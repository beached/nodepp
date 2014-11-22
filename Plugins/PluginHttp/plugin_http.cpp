
#include <cinttypes>
#include <string>

#include "plugin_http.h"
#include <boost/shared_ptr.hpp>

namespace daw {
	namespace nodepp {
		namespace plugins {
			std::string HttpServer::name( ) const {
				return "HttpServer";
			}

			int64_t HttpServer::version( ) const {
				return 201411211500;
			}
		}	// namespace plugins
	}	// namespace nodepp
}	// namespace daw
