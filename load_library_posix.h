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
