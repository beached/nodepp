// The MIT License (MIT)
// 
// Copyright (c) 2014-2015 Darrell Wright
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
					std::string msg = "Could not load function in library err : " + err;
					throw std::runtime_error( msg );
				}
				return function_ptr;
			}
		} // namespace impl
	}	// namespace system
}	// namespace daw
