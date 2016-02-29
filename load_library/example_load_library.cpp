#include <algorithm>
#include <boost/filesystem.hpp>
#include <iostream>
#include <memory>
#include <sstream>

#include "load_library.h"
#include "plugin_base.h"
#include "range_algorithm.h"

using namespace daw::algorithm;

std::vector<boost::filesystem::path> get_files_in_folder( std::string folder, std::vector<std::string> const & extensions ) {
	namespace fs = boost::filesystem;
	auto result = std::vector<fs::path> { };
	auto p = fs::path( folder.c_str( ) );

	if( fs::exists( p ) && fs::is_directory( p ) ) {
		std::copy_if( fs::directory_iterator( folder ), fs::directory_iterator( ), std::back_inserter( result ), [&extensions]( fs::path const & path ) {
			return fs::is_regular_file( path ) && (extensions.empty( ) || contains( extensions, fs::extension( path ) ));
		} );
	}
	return sort( result );
}

using plugin_t = std::pair <daw::system::LibraryHandle, std::unique_ptr<daw::nodepp::plugins::IPlugin>>;

struct test {
// 	int f() const volatile {
// 		return 1 + 1;
// 	}
	int blah;
	test( int b ): blah( b ) { }

	int f( ) const {
		return 1 + 1;
	}

	int g( ) {
		return ++blah;
	}

	uint32_t s( std::string bblah ) {
		uint32_t count = 0;
		for( auto it = bblah.begin( ); it != bblah.end( ); ++it ) {
			count += static_cast<uint32_t>( *it );
		}
		return count;
	}
};

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

			ss <<"Error loading plugin: " <<filename <<" with error\n" <<ex.what( ) <<std::endl;
			std::cerr <<ss.str( ) <<std::endl;
		}
	}

	return results;
}

int main( int, char const ** ) {
	std::string const plugin_folder = "../..plugins/";
	auto libraries = load_libraries_in_folder( plugin_folder );
	for( auto const & lib : libraries ) {
		const auto& library = *lib.second;
		std::cout <<"name: " <<library.name( ) <<" version: " <<library.version( ) <<std::endl;
	}

	return EXIT_SUCCESS;
}

