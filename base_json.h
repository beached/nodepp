#pragma once

#include <boost/optional.hpp>
#include <ctime>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
	
#include "daw_traits.h"

namespace daw {
	namespace nodepp {
		namespace base {
			namespace json {

				std::string enquote( std::string const & value );
				
				namespace details {
					std::string json_name( std::string const & name );
					std::string enbracket( std::string const & json_value );
				}	// namespace details

				std::string json_value( std::string const & name, std::string const & value );

				std::string json_value( std::string const & name, bool value );

				std::string json_value( std::string const & name );

				std::string json_value( std::string const & name, std::time_t value );

				template<typename Number>
				auto json_value( std::string const & name, Number const & value ) -> typename std::enable_if<daw::traits::is_numeric<Number>::value, std::string>::type {
					using std::to_string;
					return details::json_name( name ) + to_string( value );
				}				
				
				template<typename T>
				auto json_value( std::string const & name, T const & value ) -> decltype( value.serialize_to_json( ) ) {
					return details::json_name( name ) + value.serialize_to_json( );
				}

				template<typename T>
				std::string json_value( std::string const & name, boost::optional<T> const & value ) {
					if( value ) {
						return json_value( name, static_cast<T>( value.get( )) );
					} else {
						return json_value( name );
					}
				}

				template<typename T>
				auto json_value( std::string const & name, std::vector<T> const & values ) -> decltype( json_value( name, values[0] ) ) {
					std::stringstream result;
					result << details::json_name( name ) + "[\n";
					for( auto const & item : values ) {
						result << json_value( "", item ) << ",\n";
					}
					result << "\n]";
					return result.str( );
				}

				template<typename Container, typename Container::value_type, typename Container::iterator>
				std::string json_value( std::string const & name, Container const & values ) /*-> typename std::enable_if<daw::traits::is_container_or_array<Container>::value, std::string>::type*/ {
					std::stringstream result;
					result << details::json_name( name ) + "[\n";
					for( auto const & item : values ) {
						result << json_value( "", item ) << ",\n";
					}
					result << "\n]";
					return result.str( );
				}

			}	// namespace json
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw

template<typename T, typename EnableIf = decltype(std::declval<T>( ).serialize_to_json( ))>
std::ostream& operator<<(std::ostream& os, T const & data) {
	os << data.serialize_to_json( );
}
