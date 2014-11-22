
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

std::unique_ptr<daw::nodepp::plugins::HttpServer> create_plugin() {
	return std::unique_ptr<daw::nodepp::plugins::HttpServer>(new(std::nothrow) daw::nodepp::plugins::HttpServer());
}
