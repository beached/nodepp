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

#include "base_json_parser.h"
#include "daw_traits.h"
#include "utility.h"



namespace daw {
	namespace nodepp {
		namespace base {
			namespace json {
				std::string to_string( std::string const & value );
				using std::to_string;

				using namespace daw::traits;

				std::string enquote( std::string const & value );

				namespace details {
					std::string json_name( std::string const & name );
					std::string enbrace( std::string const & json_value );
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
				std::string value_to_json_timestamp( std::string const & name, std::time_t const & value );

				void json_to_value( std::string const & json_text, std::time_t & value );				

				// Template Declarations

				// boost optional.  will error out if T does not support value_to_json
				template<typename Optional>
				std::string value_to_json( std::string const & name, boost::optional<Optional> const & value );

				template<typename Optional>
				void json_to_value( std::string const & json_text, boost::optional<Optional> & value );

				template<typename T>
				void json_to_value( std::string const & json_text, std::shared_ptr<T> & value );

				template<typename T>
				void json_to_value( std::string const & json_text, std::weak_ptr<T> & value );

				// Numbers
				template<typename Number, typename std::enable_if<is_numeric<Number>::value, int>::type = 0>
				std::string value_to_json_number( std::string const & name, Number const & value ) {
					return details::json_name( name ) + to_string( value );
				}

				std::string value_to_json( std::string const & name, int const & value );

				std::string value_to_json( std::string const & name, unsigned int const & value );

				std::string value_to_json( std::string const & name, int64_t const & value );

				std::string value_to_json( std::string const & name, uint64_t const & value );

				std::string value_to_json( std::string const & name, double const & value );

				// Definitions

				template<typename Number, typename std::enable_if<is_numeric<Number>::value, int>::type = 0>
				void json_to_value( std::string const & json_text, Number & value ) {
					throw std::runtime_error( "Method not implemented" );
				}				

				// pair types
				template<typename First, typename Second>
				std::string value_to_json( std::string const & name, std::pair<First, Second> const & value ) {
					return details::json_name( name ) + "[ " + value_to_json( value.first ) + ", " + value_to_json( value.second ) + " ]";
				}

				template<typename First, typename Second>
				void json_to_value( std::string const & json_text, std::pair<First, Second> & value ) {
					throw std::runtime_error( "Method not implemented" );
				}

				template<typename T>
				void value_to_json( std::string const & name, std::shared_ptr<T> const & value ) {
					if( !value ) {
						value_to_json( name );
					}
					value_to_json( name, *value );
				}

				template<typename T>
				void value_to_json( std::string const & name, std::weak_ptr<T> const & value ) {
					if( !value.expired( ) ) {
						auto shared_value = value.lock( );
						if( !shared_value ) {
							value_to_json( name );
						}
						value_to_json( name, *shared_value );
					}
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
					throw std::runtime_error( "Method not implemented" );
				}				

				// container/array

				template<typename Key, typename Value>
				std::string value_to_json( std::string const & name, std::map<Key, Value> const & values ) {
					std::stringstream result;
					result << details::json_name( name ) + "[ ";
					bool is_first = true;
					for( auto const & item : values ) {
						result << (!is_first? ",": "");
						is_first = false;
						result << "{ " << value_to_json( "key", item.first ) << ", ";
						result << value_to_json( "value", item.second ) << " }";
					}
					result << " ]";
					return result.str( );
				}

				template<typename Key, typename Value>
				std::string value_to_json( std::string const & name, std::unordered_map<Key, Value> const & values ) {
					std::stringstream result;
					result << details::json_name( name ) + "[ ";
					bool is_first = true;
					for( auto const & item : values ) {
						result << (!is_first? ",": "");
						is_first = false;
						result << "{ " << value_to_json( "key", item.first ) << ", ";
						result << value_to_json( "value", item.second ) << " }";
					}
					result << " ]";
					return result.str( );
				}

				// container/array.
				template<typename Container, typename std::enable_if<is_container<Container>::value, long>::type = 0>
				std::string value_to_json( std::string const & name, Container const & values ) {
				std::stringstream result;
					result << details::json_name( name ) + "{ ";
					bool is_first = true;
					for( auto const & item : values ) {
						result << (!is_first? ",": "") << value_to_json( "", item );
						is_first = false;
					}
					result << " }";
					return result.str( );
				}

				template<typename Key, typename Value>
				std::string json_to_value( std::string const & json_text, std::map<Key, Value> & values ) {
					throw std::runtime_error( "Method not implemented" );
				}

				template<typename Key, typename Value>
				std::string json_to_value( std::string const & json_text, std::unordered_map<Key, Value> & values ) {
					throw std::runtime_error( "Method not implemented" );
				}

				template<typename Container, typename std::enable_if<is_container<Container>::value, long>::type = 0>
				void json_to_value( std::string const & json_text, Container & values ) {
					throw std::runtime_error( "Method not implemented" );
				}


				struct JsonLink {
					enum class Action { encode, decode };
				private:
					std::string m_name;
					using encode_function_t = std::function < void( std::string & json_text ) > ;
					using decode_function_t = std::function < void( json_obj json_values )>;

					struct bind_functions_t {
						encode_function_t encode;
						decode_function_t decode;
					};
					std::unordered_map<std::string, bind_functions_t> m_data_map;
				public:
					JsonLink( std::string name = "" );
					~JsonLink( ) = default;
					JsonLink( JsonLink const & ) = default;
					JsonLink& operator=(JsonLink const &) = default;
					JsonLink( JsonLink && other );
					JsonLink& operator=(JsonLink && rhs);


					std::string & json_object_name( );
					
					std::string const & json_object_name( ) const;

					std::string encode( ) const;
					
					void decode( boost::string_ref const json_text );
					
					void reset_jsonlink( );

					template<typename T>
					JsonLink& link_value( std::string name, T& value ) {
						T* value_ptr = &value;
						bind_functions_t bind_functions;
						bind_functions.encode = [value_ptr, name]( std::string & json_text ) {
							assert( value_ptr );
							T& val = *value_ptr;
							json_text = value_to_json( name, val );
						};

						bind_functions.decode = [value_ptr, name]( json_obj json_values ) mutable {
							assert( value_ptr );
							assert( json_values );
							T& val = *value_ptr;
							auto obj = boost::get<impl::object_value>( *json_values );
							auto member = obj.find( name );
							if( obj.end( ) == member ) {
								// TODO: determine if correct course of action
								throw std::runtime_error( "JSON object does not match expected object layout" );
							}
							if( !member->second ) {
								// TODO: determine if correct course of action
								throw std::runtime_error( "JSON object does not contain value requested" );
							}
							val = boost::get<T>( *member->second );
						};

						m_data_map[name] = std::move( bind_functions );
						
						return *this;
					}

					//JsonLink& link_timestamp( std::string name, std::time_t& value );
				};	// struct JsonLink

				std::string value_to_json( std::string const & name, JsonLink const & obj );
				void json_to_value( std::string const & json_text, JsonLink & obj );

				std::ostream& operator<<(std::ostream& os, daw::nodepp::base::json::JsonLink const & data);

				template<typename T, typename EnableIf = decltype(std::declval<T>( ).serialize_to_json( ))>
				std::ostream& operator<<(std::ostream& os, T const & data) {
					os << data.serialize_to_json( );
				}


			}	// namespace json
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw

