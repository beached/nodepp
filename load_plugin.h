#pragma once

#include <boost/filesystem.hpp>
#include <string>
#include <vector>

#include "load_library.h"
#include "plugin_base.h"

namespace daw {
	namespace nodepp {
		namespace base {			

			using plugin_t = std::pair < daw::system::LibraryHandle, std::unique_ptr<daw::nodepp::plugins::IPlugin> > ;

			std::vector<boost::filesystem::path> get_files_in_folder( std::string folder, std::vector<std::string> const& extensions );

			std::vector<plugin_t> load_libraries_in_folder( std::string plugin_folder );
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
