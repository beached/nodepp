
#include <dlfcn.h>
#include <string>

#include "load_library_posix.h"

namespace daw {
	namespace system {
		namespace impl {

			void* load_library(std::string library_path) {
				return dlopen(library_path.c_str(), RTLD_LAZY);
			}

			void* load_library( std::wstring library_path ) {
				std::runtime_error( "Unsupported interface" );
			}


			void close_library(void* handle) {
				if (nullptr != handle) {
					dlclose(handle);
				}
			}
		} // namespace impl
	}	// namespace system
}	// namespace daw
