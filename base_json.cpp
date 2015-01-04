
#include <ctime>
#include <iomanip>

#include "base_json.h"


namespace daw {
	namespace nodepp {
		namespace base {
			namespace json {
				std::string enquote( std::string const & value ) {
					return "\"" + value + "\"";
				}
				namespace details {
					std::string json_name( std::string const & name ) {
						return enquote( name ) + ": ";
					}

					std::string enbracket( std::string const & json_value ) {
						return "{\n" + json_value + "\n}";
					}
				}
				
				std::string json_value( std::string const & name, std::string const & value ) {
					return details::json_name( name ) + enquote( value );
				}

				std::string json_value( std::string const & name, bool value ) {
					return details::json_name( name ) + (value ? "true" : "false");
				}

				std::string json_value( std::string const & name ) {
					return details::json_name( name ) + "null";
				}

				std::string json_value( std::string const & name, std::time_t const & timestamp ) {
					std::tm tm = { 0 };
#ifdef WIN32	// really, why not agree on the same name
					localtime_s( &tm, &timestamp );
#else
					localtime_r( &tm, &timestamp );
#endif
					std::stringstream ss;
					ss << "\"" << std::put_time( &tm, "%a %b %d %Y %T GMT%z (%Z)" ) << "\"";
					return details::json_name( name ) + ss.str( );
				}

			}	// namespace json
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
