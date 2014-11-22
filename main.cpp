#include <algorithm>
#include <cstdlib>
#include <boost/filesystem.hpp>
#include "range_algorithm.h"
#include "plugin_base.h"
#include "load_library.h"

using namespace daw::algorithm;

std::vector<boost::filesystem::path> get_files_in_folder( std::string folder, std::vector<std::string> const& extensions ) {
	namespace fs = boost::filesystem;
	auto result = std::vector<fs::path>{ };
	auto p = fs::path( folder.c_str( ) );

	if( fs::exists( p ) && fs::is_directory( p ) ) {
		
		std::copy_if( fs::directory_iterator( folder ), fs::directory_iterator( ), std::back_inserter( result ), [&extensions]( fs::path const & path ) {
			return fs::is_regular_file( path ) && (extensions.empty( ) || contains( extensions, fs::extension( path ) ));
		} );
	}
	return sort( result );
}

using plugin_t = std::pair < daw::system::LibraryHandle, std::unique_ptr<daw::nodepp::plugins::IPlugin> > ;

std::vector<plugin_t> load_libraries_in_folder( std::string plugin_folder ) {
	static std::vector<std::string> const extensions = { ".npp" };

	std::vector<plugin_t> results;
	for( auto const & plugin_file : get_files_in_folder( plugin_folder, extensions ) ) {
		const auto& filename = plugin_file.relative_path( ).string( );
		try {
			auto handle = daw::system::LibraryHandle( filename );
			auto plugin = handle.call_function<std::unique_ptr<daw::nodepp::plugins::IPlugin>>( "create_plugin" );
			results.emplace_back( std::move( handle ), std::move( plugin ) );
		} catch( std::runtime_error const & ex ) {
			// log better
			std::cerr << "Error loading plugin: " << filename << " with error\n" << ex.what( ) << std::endl;
		}
	}

	return results;

}

int main( int, char const ** ) {
	std::string const plugin_folder = ".\\plugins\\";
	auto libraries = load_libraries_in_folder( plugin_folder );



	return EXIT_SUCCESS;
}
