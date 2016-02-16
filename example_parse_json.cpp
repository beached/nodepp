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
#include <boost/iostreams/device/mapped_file.hpp>
#include <iostream>
#include <chrono>

int main( int argc, char** argv ) {
	if( argc <= 1 ) {
		std::cerr <<"Must supply a json file" <<std::endl;
		exit( EXIT_FAILURE );
	}


	boost::iostreams::mapped_file_source json_str;
	json_str.open( argv[1] );
	if( !json_str.is_open( ) ) {
		std::cerr <<"Error opening file: " <<argv[1] <<std::endl;
		exit( EXIT_FAILURE );
	}

	using namespace daw::json::impl;
	using namespace daw::json;

	std::chrono::time_point<std::chrono::system_clock> start, end;

	auto file_size = json_str.size( );

	auto json = parse_json( json_str.begin( ), json_str.end( ) );
	end = std::chrono::system_clock::now( );

	std::chrono::duration<double> elapsed_seconds = end - start;

	std::cout << "Total time: " << elapsed_seconds.count( ) << " total bytes: " << file_size << " speed: " << (static_cast<double>( file_size ) / elapsed_seconds.count( )) << std::endl;

	if( json.is_null( ) ) {
		std::cerr <<"Could not find data" <<std::endl;
		exit( EXIT_FAILURE );
	}
	
	//std::cout << "value: " << json <<"\n";
	// system( "pause" );
	return EXIT_SUCCESS;
}

