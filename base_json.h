#pragma once

#include <boost/optional.hpp>
#include <ctime>
#include <cstdint>
#include <unordered_map>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "daw_traits.h"
#include "utility.h"

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

				namespace details {
// 					template<typename T, typename = int>
// 					struct has_serialize: std::false_type { };
// 
// 					template<typename T>
// 					struct has_serialize<T, decltype((void)T::serialize_to_json, 0)> : std::true_type { };
					//GENERATE_HAS_MEMBER( serialize_to_json, std::string );
					GENERATE_HAS_MEMBER( deserialize_from_json, std::string );

					template <typename T>
					struct has_serialize_to_json {
						template <
							typename U,
							typename S = decltype (((U*)0)->serialize_to_json( ))
						>
						static char test( U* u );
						template <typename U> static long test( ... );
						enum { value = sizeof test<T>( 0 ) == 1 };
					};
				}	// namespace details

				// Template Declarations
 				//Numbers
				template<typename Number, typename std::enable_if<daw::traits::is_numeric<Number>::value>::type>
				std::string value_to_json( std::string const & name, Number const & value );

				template<typename Number, typename std::enable_if<daw::traits::is_numeric<Number>::value>::type>
				void json_to_value( std::string const & json_text, Number & value );

				// any object with a serialize_to_json method
				template<typename T>
				typename std::enable_if< details::has_serialize_to_json<T>::value, std::string>::type value_to_json( std::string const & name, T const & value );

				template<typename T, typename std::enable_if< details::has_deserialize_from_json<T>::value>::type>
				void json_to_value( std::string const & json_text, T & value );

				// boost optional.  will error out if T does not support value_to_json
				template<typename T>
				std::string value_to_json( std::string const & name, boost::optional<T> const & value );

				template<typename T>
				void json_to_value( std::string const & json_text, boost::optional<T> & value );

				// array.
				template<typename T, typename std::enable_if<daw::traits::is_container_or_array<T>::value>::type>
				std::string value_to_json( std::string const & name, T const & values );

				template<typename T, typename std::enable_if<daw::traits::is_container_or_array<T>::value>::type>
				void json_to_value( std::string const & json_text, T & values );

				// Definitions
				// Numbers
				template<typename Number, typename std::enable_if<daw::traits::is_numeric<Number>::value>::type>
				std::string value_to_json( std::string const & name, Number const & value )  {
					using std::to_string;
					return details::json_name( name ) + to_string( value );
				}

				template<typename Number, typename std::enable_if<daw::traits::is_numeric<Number>::value>::type>
				void json_to_value( std::string const & json_text, Number & value ) {
					//TODO
				}
				
				// any object with a serialize_to_json method
				template<typename T, typename C/*, typename std::enable_if< details::has_serialize_to_json<T>::value>::type*/>
				typename std::enable_if< details::has_serialize_to_json<T>::value, std::string>::type value_to_json( std::string const & name, T const & value ) {
					return details::json_name( name ) + value.serialize_to_json( );
				}

				template<typename T, typename std::enable_if< details::has_deserialize_from_json<T>::value>::type>
				void json_to_value( std::string const & json_text, T & value )  {
					// TODO
				}

				// boost optional.  will error out if T does not support value_to_json
				template<typename T>
				std::string value_to_json( std::string const & name, boost::optional<T> const & value ) {
					if( value ) {
						return value_to_json( name, value.get( ) );
					} else {
						return value_to_json( name );
					}
				}

				template<typename T>
				void json_to_value( std::string const & json_text, boost::optional<T> & value ) {
					// TODO
				}

				// array.
				template<typename T, typename std::enable_if<daw::traits::is_container_or_array<T>::value>::type>
				std::string value_to_json( std::string const & name, T const & values ) {
					std::stringstream result;
					result << details::json_name( name ) + "[\n";
					for( auto const & item : values ) {
						result << value_to_json( "", item ) << ",\n";
					}
					result << "\n]";
					return result.str( );
				}

				template<typename T, typename std::enable_if<daw::traits::is_container_or_array<T>::value>::type>
				void json_to_value( std::string const & json_text, T & values ) {
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
						m_data_map[name] = [value_ptr, name]( std::string & json_text, Action action ) {
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
