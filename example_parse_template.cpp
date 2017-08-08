// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Darrell Wright
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

#include <boost/utility/string_view.hpp>
#include <cstdlib>
#include <iostream>
#include <daw/parse_template/daw_parse_template.h>
#include <daw/daw_memory_mapped_file.h>

int main( int argc, char const ** argv ) {
	using namespace daw::parse_template;

	if( argc <= 1 ) {
		std::cerr << "Must supply a template file" << std::endl;
		exit( EXIT_FAILURE );
	}

	daw::filesystem::memory_mapped_file_t<char> template_str( argv[1] );
	if( !template_str.is_open( ) ) {
		std::cerr << "Error opening file: " << argv[1] << std::endl;
		exit( EXIT_FAILURE );
	}

	boost::string_view str { template_str.begin( ), template_str.size( ) };

	auto p = create_parse_template( template_str.begin( ), template_str.end( ) );

	for( auto const & t : p.list_callbacks( ) ) {
		std::cout << t << "\n";
	}
	boost::string_view t;

	p.add_callback( daw::range::create_char_range( "dummy_text_cb" ), []( ) { return std::string { "This is some dummy text" }; } );
	p.add_callback( daw::range::create_char_range( "repeat_test" ), []( ) {
		std::vector<std::string> result;
		std::string s;
		for( size_t n=0; n<10; ++n ) {
			result.push_back( s + static_cast<char>('0' + n) );
		}
		return result;
	} );
	std::cout << "\n---\n";
	p.process_template( std::cout );
	std::cout << "\n---\n";
	return EXIT_SUCCESS;
}

