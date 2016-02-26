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

#include <cstdlib>

#include "daw_parse_template.h"
#include <iostream>


int main( int, char const ** ) {
	using namespace daw::parse_template;

	std::string str = R"raw(<%="test_cb"%>
<%date%>
<%date_format="%Y"%>
<%date%>
<%repeat="test_repeat"%>
)raw";

	ParseTemplate p( str );

	for( auto const & t : p.list_callbacks( ) ) {
		std::cout << t << "\n";
	}
	p.add_callback( "test_cb", []( ) { return std::string{ "callback test\n" }; } );
	p.add_callback( "test_repeat", []( ) {
		return std::vector<std::string>{ 10, "test" };
	} );
	std::cout << "template before:\n" << str << "\n---\ntemplate after:\n" << p.process_template( ) << "\n---\n";
	return EXIT_SUCCESS;
}

