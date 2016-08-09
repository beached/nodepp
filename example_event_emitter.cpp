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

#include <cstdlib>
#include <iostream>
#include <future>
#include <daw/nodepp/base_event_emitter.h>
#include <daw/nodepp/base_service_handle.h>

template<typename ValueType, typename Function>
void sequence( ValueType const & low, ValueType const & high, ValueType inc, Function op ) {
	assert( low <= high );
	ValueType n = low;
	while( n <= high ) {
		op( n );
		n += inc;
	}
}

constexpr double PI( ) {
	return 3.141592653589793238463;
}

int main( int, char const ** ) {
	using namespace daw::nodepp;

	auto event_emitter = base::create_event_emitter( );

	event_emitter->on( "calc_area", [event_emitter]( double r ) {
		event_emitter->emit( "conv_sqft_to_sqm", r*r*PI( ) );
	} );

	event_emitter->on( "conv_sqft_to_sqm", [event_emitter]( double A ) {
		event_emitter->emit( "done", A*0.092903 );
	} );

	event_emitter->on( "done", []( double Asqm ) {
		std::cout <<Asqm <<std::endl;
	} );

	auto srv = std::async( [event_emitter]( ) {
		base::start_service( base::StartServiceMode::Single );
	} );

	sequence( 1.0, 1000.0, 1.0, [event_emitter]( double val ) {
		event_emitter->emit( "calc_area", val );
	} );

	srv.wait( );
	return EXIT_SUCCESS;
}

