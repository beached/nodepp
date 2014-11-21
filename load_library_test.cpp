
#include "load_library.h"
#include <windows.h>
#include <iostream>

int main( int, char** ) {

	char* strA = "this is a test";
	char* strB = "is";

	auto result = daw::system::call_dll_function<int>( "User32.dll", "MessageBoxA", (HWND)NULL, (LPCSTR)strA, (LPCSTR)strB, (UINT)0 );

	std::wcout << result << std::endl;
	system( "PAUSE" );
	return 0;
}