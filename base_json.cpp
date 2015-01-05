
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
				
				std::string value_to_json( std::string const & name, std::string const & value ) {
					return details::json_name( name ) + enquote( value );
				}

				std::string value_to_json( std::string const & name, bool value ) {
					return details::json_name( name ) + (value ? "true" : "false");
				}

				std::string value_to_json( std::string const & name ) {
					return details::json_name( name ) + "null";
				}

				std::string value_to_json( std::string const & name, std::time_t const & timestamp ) {
					return details::json_name( name ) + enquote( to_string( timestamp ) );
				}

				JsonLink::JsonLink( std::string name ): m_name( std::move( name ) ), m_data_map( ) { }				

			}	// namespace json
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
