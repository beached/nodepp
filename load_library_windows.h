#pragma once

#include <codecvt>
#include <locale>
#include <string>
#include "utility.h"
#include <windows.h>

namespace daw {
	namespace system {
		namespace impl {
			HINSTANCE load_library(std::wstring library_path);
			HINSTANCE load_library(std::string library_path);
			void close_library(HINSTANCE handle);


			template<typename ResultType, typename... ArgTypes>
			daw::function_pointer_t<ResultType, ArgTypes...> get_function_address(const HINSTANCE& handle, std::string&& function_name) {
				using function_ptr_t = daw::function_pointer_t<ResultType, ArgTypes...> ;
				auto function_ptr = reinterpret_cast<function_ptr_t>(GetProcAddress(handle, function_name.c_str()));
				if (!function_ptr) {
					auto err = GetLastError();
					throw std::runtime_error("Could not load function in library");
				}
				return function_ptr;
			}
		} // namespace impl
	}	// namespace system
}	// namespace daw
