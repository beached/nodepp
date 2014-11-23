#include <codecvt>
#include <locale>
#include <sstream>
#include <string>
#include <type_traits>
#include <windows.h>

#include "load_library_windows.h"
#include <utility>

namespace daw {
	namespace system {
		namespace impl {
			std::wstring widen_string(std::string in_str) {
				static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
				return converter.from_bytes(in_str);
			}

			std::string GetLastErrorAsString(DWORD errorMessageID) {
				//Get the error message, if any.
				if( errorMessageID == 0 ) {
					return "No error message has been recorded";
				}
				LPSTR messageBuffer = nullptr;
				auto size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

				std::string message{ messageBuffer, size };

				//Free the buffer.
				LocalFree(messageBuffer);

				return message;
			}

			std::pair<DWORD, std::string> GetLastErrorAsString( ) {
				unsigned long err_no = ::GetLastError( );
				return std::make_pair( err_no, GetLastErrorAsString( err_no ) );
			}

			HINSTANCE load_library(std::wstring library_path) {
				auto result = static_cast<HINSTANCE>(LoadLibraryW(library_path.c_str()));
				if (!result) {
					std::stringstream msg;
					auto error_info = GetLastErrorAsString( );
					msg << "Could not open library: error no: " << error_info.first << " with message: " << error_info.second;
					throw std::runtime_error( msg.str( ) );
				}
				return result;
			}

			HINSTANCE load_library(std::string library_path) {
				return load_library(widen_string(std::move(library_path)));
			}

			void close_library(HINSTANCE handle) {
				if (nullptr != handle) {
					FreeLibrary(handle);
				}
			}

		} // namespace impl
	}	// namespace system
}	// namespace daw
