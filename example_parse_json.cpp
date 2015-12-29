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

#include "parse_json/daw_json_parser.h"
#include <cstdlib>
#include <sys/mman.h>
#include <cstdio>
#include <unistd.h>
#include <err.h>
#include <fcntl.h>
#include <iostream>

int main( int argc, char** argv ) {
	assert( argc > 1 || "Must supply a json file" );
	int fd = -1;
	if( (fd = open( argv[1], O_RDONLY, 0 )) == -1 ) {
		err( 1, "open" );
	}

	char const * const json_str = static_cast<char*>( mmap( nullptr, 4096, PROT_READ, MAP_FILE|MAP_PRIVATE, fd, 0 ) );

	if( MAP_FAILED == json_str ) {
		errx( 1, "mmap" );
	}


	using namespace daw::json::impl;
	using namespace daw::json;

	std::cout << json_str << std::endl;

	auto json = parse_json( json_str );
	if( !json ) {
		std::cerr << "Could not find data" << std::endl;
		exit( EXIT_FAILURE );
	}

	std::cout << "value: " << daw::json::get<std::string>( *json ) << "\n";
	switch( json->type( ) ) {
	case value_t::value_types::integral:
		std::cout << json->get_integral( ) << "\n";
		break;
	case value_t::value_types::real:
		std::cout << json->get_real( ) << "\n";
		break;
	case value_t::value_types::boolean:
		std::cout << json->get_boolean( ) << "\n";
		break;
	case value_t::value_types::string:
		std::cout << "'" << json->get_string( ) << "'\n";
		break;
	case value_t::value_types::null:
		std::cout << "null\n";
		break;
	default:
		std::cout << "other\n";
	}	

	munmap( (void*)json_str, 4096 );
	close( fd );

	return EXIT_SUCCESS;
}

