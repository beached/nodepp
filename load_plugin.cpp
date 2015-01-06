
#include <boost/filesystem.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

#include "load_library.h"
#include "plugin_base.h"
#include "range_algorithm.h"
#include "load_plugin.h"

namespace daw {
	namespace nodepp {
		namespace base {
			using namespace daw::algorithm;

			using plugin_t = std::pair < daw::system::LibraryHandle, std::unique_ptr<daw::nodepp::plugins::IPlugin> > ;

			std::vector<boost::filesystem::path> get_files_in_folder( std::string folder, std::vector<std::string> const& extensions ) {
				namespace fs = boost::filesystem;
				auto result = std::vector < fs::path > { };
				auto p = fs::path( folder.c_str( ) );

				if( fs::exists( p ) && fs::is_directory( p ) ) {

					std::copy_if( fs::directory_iterator( folder ), fs::directory_iterator( ), std::back_inserter( result ), [&extensions]( fs::path const & path ) {
						return fs::is_regular_file( path ) && (extensions.empty( ) || contains( extensions, fs::extension( path ) ));
					} );
				}
				return sort( result );
			}


			std::vector<plugin_t> load_libraries_in_folder( std::string plugin_folder ) {
				static std::vector<std::string> const extensions = { ".npp" };

				std::vector<plugin_t> results;
				for( auto const & plugin_file : get_files_in_folder( plugin_folder, extensions ) ) {
					const auto& filename = plugin_file.relative_path( ).string( );
					try {
						auto handle = daw::system::LibraryHandle( filename );
						auto create_func = handle.get_function<daw::nodepp::plugins::IPlugin*>( "create_plugin" );
						auto plugin = std::unique_ptr<daw::nodepp::plugins::IPlugin>( create_func( ) );
						results.emplace_back( std::move( handle ), std::move( plugin ) );
					} catch( std::runtime_error const & ex ) {
						// We are going to keep on going if we cannot load a plugin
						std::stringstream ss;

						ss << "Error loading plugin: " << filename << " with error\n" << ex.what( ) << std::endl;
						std::cerr << ss.str( ) << std::endl;
					}
				}

				return results;
			}
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
