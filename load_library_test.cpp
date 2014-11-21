
#include "load_library.h"
#include <iostream>

int main( int, char** ) {

	#ifdef _WIN32
	char* strA = "this is a test";
	char* strB = "is";

	auto result = daw::system::call_dll_function<int>( "User32.dll", "MessageBoxA", (HWND)NULL, (LPCSTR)strA, (LPCSTR)strB, (UINT)0 );
	#else
	auto result = daw::system::call_dll_function<std::string>( "./libtestlib.so", "test", std::string{ "this is a test" } );
	#endif
	
	std::cout << result << std::endl;
	return 0;
}
