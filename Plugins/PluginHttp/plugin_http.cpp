
#include <cinttypes>
#include <string>

#include <boost/shared_ptr.hpp>
#include "plugin_http.h"

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

#ifdef _WIN32
#include <windows.h>
BOOL APIENTRY DllMain( HANDLE, DWORD, LPVOID ) {
	return TRUE;
}

#define MAKEDLL extern "C" __declspec(dllexport)
#else
#define MAKEDLL
#endif

MAKEDLL daw::nodepp::plugins::IPlugin* create_plugin( ) {
	return new daw::nodepp::plugins::HttpServer( );
}
