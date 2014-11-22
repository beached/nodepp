#include <algorithm>
#include <cstdlib>
#include <boost/filesystem.hpp>
#include <iostream>
#include <string>
#include <vector>

template<typename Container>
Container& sort( Container& container ) {
	std::sort( std::begin( container ), std::end( container ) );
	return container;
}

template<typename Container, typename StringType>
bool contains( Container const & container, StringType const & key ) {
	return std::find( std::begin( container ), std::end( container ), key ) != std::end( container );	
}

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

int main( int const argc, char const ** argv ) {
	std::string const plugin_folder = ".\\plugins\\";
	std::vector<std::string> extensions = { ".npp" };
	for( auto const & plugin_file : get_files_in_folder( plugin_folder, extensions ) ) {
		std::cout << plugin_file.filename( ) << std::endl;
	}



	return EXIT_SUCCESS;
}