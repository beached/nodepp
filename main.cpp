#include <algorithm>
#include <cstdlib>
#include <boost/filesystem.hpp>
#include "range_algorithm.h"

std::vector<boost::filesystem::path> get_files_in_folder( std::string folder, std::vector<std::string> const& extensions );

using namespace daw::algorithm;

std::vector<boost::filesystem::path> get_files_in_folder( std::string folder, std::vector<std::string> const& extensions ) {
	namespace fs = boost::filesystem;
	auto result = std::vector<fs::path>{ };
	auto p = fs::path( folder );	

	if( fs::exists( p ) && fs::is_directory( p ) ) {
		
		std::copy_if( fs::directory_iterator( folder ), fs::directory_iterator( ), std::back_inserter( result ), [&extensions]( fs::path const & path ) {
			return fs::is_regular_file( path ) && (extensions.empty( ) || contains( extensions, fs::extension( path ) ));
		} );
	}
	return sort( result );
}

int main( int, char const ** ) {
	std::string const plugin_folder = ".\\plugins\\";
	std::vector<std::string> extensions = { ".npp" };
	for( auto const & plugin_file : get_files_in_folder( plugin_folder, extensions ) ) {
		std::cout << plugin_file.filename( ) << std::endl;
	}



	return EXIT_SUCCESS;
}
