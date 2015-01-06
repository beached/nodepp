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

#include <cassert>
#include <ctime>
#include <iomanip>

#include "base_json.h"

#ifndef WIN32
void localtime_s( struct tm* result, std::time_t const * source ) {
	localtime_r( source, result );
}
#endif

std::string to_string( std::time_t const & timestamp, std::string format = "%a %b %d %Y %T GMT%z (%Z)" ) {
	char buffer[200];
	std::tm tm = { };
	localtime_s( &tm, &timestamp );
	auto count = std::strftime( buffer, 200, format.c_str( ), &tm );
	assert( count < 200 );
	return std::string( buffer, buffer + count + 1 );
}

namespace daw {
	namespace nodepp {
		namespace base {
			namespace json {
				std::string enquote( std::string const & value ) {
					return "\"" + value + "\"";
				}

				namespace details {
					std::string json_name( std::string const & name ) {
						if( !name.empty( ) ) {
							return enquote( name ) + ": ";
						}
						return std::string( );
					}

					std::string enbracket( std::string const & json_value ) {
						return "{\n" + json_value + "\n}";
					}
				}


				// string
				std::string value_to_json( std::string const & name, std::string const & value ) {
					return details::json_name( name ) + enquote( value );
				}

				void json_to_value( std::string const & json_text, std::string & value ) {
					throw std::runtime_error( "Method not implemented" );
				}

				// bool
				std::string value_to_json( std::string const & name, bool value ) {
					return details::json_name( name ) + (value ? "true" : "false");
				}

				void json_to_value( std::string const & json_text, bool& value ) {
					throw std::runtime_error( "Method not implemented" );
				}

				// null
				std::string value_to_json( std::string const & name ) {
					return details::json_name( name ) + "null";
				}

				// date -> actaually a string, but it javascript date format encodes the date
				std::string value_to_json( std::string const & name, std::time_t const & timestamp ) {
					return details::json_name( name ) + enquote( to_string( timestamp ) );
				}

				void json_to_value( std::string const & json_text, std::time_t & value ) {
					throw std::runtime_error( "Method not implemented" );
				}

				JsonLink::JsonLink( std::string name ): m_name( std::move( name ) ), m_data_map( ) { 
					throw std::runtime_error( "Method not implemented" );
				}

			}	// namespace json
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
