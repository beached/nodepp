#include <codecvt>
#include <locale>
#include <string>
#include <type_traits>
#include <windows.h>

#include "load_library_windows.h"

namespace daw {
	namespace system {
		namespace impl {
			std::wstring widen_string(std::string in_str) {
				static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
				return converter.from_bytes(in_str);
			}

			HINSTANCE load_library(std::wstring library_path) {
				auto result = static_cast<HINSTANCE>(LoadLibraryW(library_path.c_str()));
				if (!result) {
					std::string msg = "Could not open library: error ";
					msg += GetLastError();
					throw std::runtime_error( msg );
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
