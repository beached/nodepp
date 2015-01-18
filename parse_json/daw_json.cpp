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

#include "daw_json.h"
#include "daw_json_parser.h"

#ifndef WIN32
void localtime_s( struct tm* result, std::time_t const * source ) {
	localtime_r( source, result );
}
#endif

namespace daw {
	namespace json {
		using namespace generate;
		std::string ts_to_string( std::time_t const & timestamp, std::string format ) {
			char buffer[200];
			std::tm tm = { };
			localtime_s( &tm, &timestamp );
			auto count = std::strftime( buffer, 200, format.c_str( ), &tm );
			assert( count < 200 );
			return std::string( buffer, buffer + count + 1 );
		}

		/*		std::string to_string( std::string const & value ) {
					return value;
					}
					*/
		std::string enquote( boost::string_ref value ) {
			return "\"" + value.to_string( ) + "\"";
		}

		namespace details {
			std::string json_name( boost::string_ref name ) {
				if( !name.empty( ) ) {
					return enquote( name ) + ": ";
				}
				return std::string( );
			}

			std::string enbrace( boost::string_ref json_value ) {
				return "{ " + json_value.to_string( ) + " }";
			}
		}

		namespace generate {
			// string
			std::string value_to_json( std::string const & name, std::string const & value ) {
				return details::json_name( name ) + enquote( value );
			}

			// bool
			std::string value_to_json( std::string const & name, bool value ) {
				return details::json_name( name ) + (value ? "true" : "false");
			}

			// null
			std::string value_to_json( std::string const & name ) {
				return details::json_name( name ) + "null";
			}

			// date -> actaually a string, but it javascript date format encodes the date
			// 				std::string value_to_json_timestamp( std::string const & name, std::time_t const & timestamp ) {
			// 					using daw::json::to_string;
			// 					return details::json_name( name ) + enquote( to_string( timestamp ) );
			// 				}

			std::string value_to_json( std::string const & name, int const & value ) {
				return value_to_json_number( name, value );
			}

			std::string value_to_json( std::string const & name, unsigned int const & value ) {
				return value_to_json_number( name, value );
			}

			std::string value_to_json( std::string const & name, int64_t const & value ) {
				return value_to_json_number( name, value );
			}

			std::string value_to_json( std::string const & name, uint64_t const & value ) {
				return value_to_json_number( name, value );
			}

			std::string value_to_json( std::string const & name, double const & value ) {
				return value_to_json_number( name, value );
			}
		}	// namespace generate

		namespace details {
			// String
			void json_to_value( std::string & to, impl::value_t const & from ) {
				to = from.get_string( );
			}

			// Boolean
			void json_to_value( bool & to, impl::value_t const & from ) {
				to = from.get_boolean( );
			}

			// Number, integer
			void json_to_value( int64_t & to, impl::value_t const & from ) {
				to = from.get_integral( );
			}

			// Number, real
			void json_to_value( double & to, impl::value_t const & from ) {
				to = from.get_real( );
			}

			void json_to_value( float & to, impl::value_t const & from ) {
				to = static_cast<float>(from.get_real( ));
			}
		}	// namespace details
	}	// namespace json
}	// namespace daw1
