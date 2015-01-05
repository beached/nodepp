#pragma once

#include <boost/optional.hpp>
#include <ctime>
#include <cstdint>
#include <unordered_map>
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

				// string
				std::string value_to_json( std::string const & name, std::string const & value );
				void json_to_value( std::string const & json_text, std::string & value );

				// bool
				std::string value_to_json( std::string const & name, bool value );
				void json_to_value( std::string const & json_text, bool & value );
				// null
				std::string value_to_json( std::string const & name );

				// date -> actaually a string, but it javascript date format encodes the date
				std::string value_to_json( std::string const & name, std::time_t value );
				void json_to_value( std::string const & json_text, std::time_t & value );

				// Numbers
				template<typename Number>
				auto value_to_json( std::string const & name, Number const & value ) -> typename std::enable_if<daw::traits::is_numeric<Number>::value, std::string>::type;

				template<typename Number>
				auto json_to_value( std::string const & json_text, Number & value ) -> typename std::enable_if<daw::traits::is_numeric<Number>::value>::type;

				// any object with a serialize_to_json method
				template<typename T>
				auto value_to_json( std::string const & name, T const & value ) -> decltype(value.serialize_to_json( ));

				template<typename T>
				auto json_to_value( std::string const & json_text, T & value ) -> decltype(value.deserialize_from_json( value ));

				// boost optional.  will error out if T does not support value_to_json
				template<typename T>
				auto value_to_json( std::string const & name, boost::optional<T> const & value )-> typename std::enable_if< sizeof( decltype(value_to_json( value.get( ) )) ) != 0, std::string>::type;

				template<typename T>
				auto json_to_value( std::string const & json_text, boost::optional<T> & value ) -> typename std::enable_if< sizeof( decltype(value_to_json( value.get( ) )) ) != 0, std::string>::type;

				// array.
				template<typename T>
				auto value_to_json( std::string const & name, T const & values ) -> decltype(value_to_json( name, *std::begin( values ) ));

				template<typename T>
				auto json_to_value( std::string const & json_text, T & values ) -> decltype(value_to_json( name, *std::begin( values ) ));

				// Numbers
				template<typename Number>
				auto value_to_json( std::string const & name, Number const & value ) -> typename std::enable_if<daw::traits::is_numeric<Number>::value>::type {
					using std::to_string;
					return details::json_name( name ) + to_string( value );
				}

				template<typename Number>
				auto json_to_value( std::string const & json_text, Number & value ) -> typename std::enable_if<daw::traits::is_numeric<Number>::value>::type {
					//TODO
				}

				// any object with a serialize_to_json method
				template<typename T>
				auto value_to_json( std::string const & name, T const & value ) -> decltype(value.serialize_to_json( )) {
					return details::json_name( name ) + value.serialize_to_json( );
				}

				template<typename T>
				auto json_to_value( std::string const & json_text, T & value ) -> decltype(value.deserialize_from_json( value )) {
					// TODO
				}

				// boost optional.  will error out if T does not support value_to_json
				template<typename T>
				auto value_to_json( std::string const & name, boost::optional<T> const & value )-> typename std::enable_if< sizeof( decltype(value_to_json( value.get( ) )) ) != 0, std::string>::type {
					if( value ) {
						return value_to_json( name, value.get( ) );
					} else {
						return value_to_json( name );
					}
				}

				template<typename T>
				auto json_to_value( std::string const & json_text, boost::optional<T> & value ) -> typename std::enable_if< sizeof( decltype(value_to_json( value.get( ) )) ) != 0, std::string>::type {
					// TODO
				}


				// array.
				template<typename T>
				auto value_to_json( std::string const & name, T const & values ) -> decltype(value_to_json( name, *values.begin( ) )) {
					std::stringstream result;
					result << details::json_name( name ) + "[\n";
					for( auto const & item : values ) {
						result << value_to_json( "", item ) << ",\n";
					}
					result << "\n]";
					return result.str( );
				}

				template<typename T>
				auto json_to_value( std::string const & json_text, T & values ) -> decltype(value_to_json( name, *std::begin( values ) )) {
					// TODO
				}


				struct JsonLink {
					enum class Action { encode, decode };
				private:
					std::string m_name;
					std::unordered_map<std::string, std::function<void( std::string &value, std::string & json_text, Action )>> m_data_map;
				public:
					JsonLink( std::string name = "" );

					std::string encode( );
					void decode( std::string const & json_text );

					template<typename T>
					JsonLink& link_value( std::string name, T& value ) {
						auto value_ptr = &value;
						m_data_map[name] = [value_ptr, name]( std::string & value, std::string & json_text, Action action ) {
							switch( action ) {
							case Action::encode:
								assert( value_ptr != nullptr );
								json_text = value_to_json( name, *value_ptr );
								break;
							case Action::decode:
								json_to_value( json_text, *value_ptr );
								break;
							}
						};
						return *this;
					}

				};	// struct JsonLink

			}	// namespace json
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw

template<typename T, typename EnableIf = decltype(std::declval<T>( ).serialize_to_json( ))>
std::ostream& operator<<(std::ostream& os, T const & data) {
	os << data.serialize_to_json( );
}
