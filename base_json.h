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
				
				using namespace daw::traits;

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
				std::string value_to_json( std::string const & name, std::time_t const & value );
				void json_to_value( std::string const & json_text, std::time_t & value );				

				// Template Declarations
				//Numbers
				template<typename Number>
				auto value_to_json( std::string const & name, Number const & value ) -> typename std::enable_if<is_numeric<Number>::value, std::string>::type;

				template<typename Number>
				auto json_to_value( std::string const & json_text, Number & value ) -> typename std::enable_if<is_numeric<Number>::value>::type;

				// boost optional.  will error out if T does not support value_to_json
				template<typename Optional>
				std::string value_to_json( std::string const & name, boost::optional<Optional> const & value );

				template<typename Optional>
				void json_to_value( std::string const & json_text, boost::optional<Optional> & value );

				// container/array.
				template<typename Container>
				auto value_to_json( std::string const & name, Container const & values ) -> typename std::enable_if<is_container_or_array<Container>::value, std::string>::type;

				// Definitions
				// Numbers
				template<typename Number>
				auto value_to_json( std::string const & name, Number const & value ) -> typename std::enable_if<is_numeric<Number>::value, std::string>::type {
					using std::to_string;
					return details::json_name( name ) + to_string( value );
				}

				template<typename Number>
				auto json_to_value( std::string const & json_text, Number & value ) -> typename std::enable_if<is_numeric<Number>::value>::type {
					//TODO
				}				

				// boost optional.  will error out if T does not support value_to_json
				template<typename Optional>
				std::string value_to_json( std::string const & name, boost::optional<Optional> const & value ) {
					if( value ) {
						return value_to_json( name, value.get( ) );
					} else {
						return value_to_json( name );
					}
				}

				template<typename Optional>
				void json_to_value( std::string const & json_text, boost::optional<Optional> & value ) {
					// TODO
				}

				// container/array.
				template<typename Container>
				auto value_to_json( std::string const & name, Container const & values ) -> typename std::enable_if<is_container_or_array<Container>::value, std::string>::type {
				std::stringstream result;
					result << details::json_name( name ) + "[\n";
					for( auto const & item : values ) {
						result << value_to_json( "", item ) << ",\n";
					}
					result << "\n]";
					return result.str( );
				}

				template<typename Container, typename T>
				auto json_to_value( std::string const & json_text, T & values )  -> typename std::enable_if<is_container_or_array<Container>::value>::type {
					// TODO
				}


				struct JsonLink {
					enum class Action { encode, decode };
				private:
					std::string m_name;
					std::unordered_map<std::string, std::function<void( std::string & json_text, Action )>> m_data_map;
				public:
					JsonLink( std::string name = "" );
					~JsonLink( ) = default;
					JsonLink( JsonLink const & ) = default;
					JsonLink& operator=(JsonLink const &) = default;
					JsonLink( JsonLink && other );
					JsonLink& operator=(JsonLink && rhs);


					std::string & name( );
					std::string const & name( ) const;

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
