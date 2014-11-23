

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
