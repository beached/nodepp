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

#include <dlfcn.h>
#include <string>
#include "utility.h"

namespace daw {
	namespace system {
		namespace impl {
			void* load_library(std::string library_path);

			//////////////////////////////////////////////////////////////////////////
			// Summary: Unsupported call needed to meet interface requirements
			//			Use the std::string version on posix systems
			void* load_library( std::wstring library_path );

			void close_library(void* handle);

			template<typename ResultType, typename... ArgTypes>
			daw::function_pointer_t<ResultType, ArgTypes...> get_function_address(void* handle, std::string function_name) {
				using function_ptr_t = daw::function_pointer_t < ResultType, ArgTypes... > ;
				dlerror();
				auto function_ptr = reinterpret_cast<function_ptr_t>(dlsym(handle, function_name.c_str()));
				auto has_error = dlerror();
				if (has_error) {
					throw std::runtime_error("Could not load class factory in library");
				}
				return function_ptr;
			}
		} // namespace impl
	}	// namespace system
}	// namespace daw
