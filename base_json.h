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

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <cstdint>
#include <ctime>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>
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

				std::string enquote( std::string const & value );

				namespace details {
					std::string json_name( std::string const & name );
					std::string enbrace( std::string const & json_value );
				}	// namespace details

				// string
				std::string value_to_json( std::string const & name, std::string const & value );

				// bool
				std::string value_to_json( std::string const & name, bool value );
				// null
				std::string value_to_json( std::string const & name );

				// date -> actaually a string, but it javascript date format encodes the date
				std::string value_to_json_timestamp( std::string const & name, std::time_t const & value );

				// Template Declarations

				// boost optional.  will error out if T does not support value_to_json
				template<typename Optional>
				std::string value_to_json( std::string const & name, boost::optional<Optional> const & value );

				// Numbers
				template<typename Number, typename std::enable_if<daw::traits::is_numeric<Number>::value, int>::type = 0>
				std::string value_to_json_number( std::string const & name, Number const & value ) {
					return details::json_name( name ) + to_string( value );
				}

				std::string value_to_json( std::string const & name, int const & value );

				std::string value_to_json( std::string const & name, unsigned int const & value );

				std::string value_to_json( std::string const & name, int64_t const & value );

				std::string value_to_json( std::string const & name, uint64_t const & value );

				std::string value_to_json( std::string const & name, double const & value );

				// Definitions

				// pair types
				template<typename First, typename Second>
				std::string value_to_json( std::string const & name, std::pair<First, Second> const & value ) {
					return details::json_name( name ) + "[ " + value_to_json( value.first ) + ", " + value_to_json( value.second ) + " ]";
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
				template<typename Container, typename std::enable_if<daw::traits::is_container<Container>::value, long>::type = 0>
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

				struct JsonLink;

				namespace details {
					template<typename T, typename U = T>
					void json_to_value( T & to, impl::value_t const & from ) {
						to = get<U>( from );
					}

					template<typename T, typename U = T>
					void json_to_value( boost::optional<T> & to, impl::value_t const & from ) {
						if( from.is_null( ) ) {
							to.reset( );
						} else {
							T result;
							json_to_value( result, from );
							*to = std::move( result );
						}
					}

					template<typename T, typename U = T>
					void json_to_value( std::shared_ptr<T> & to, impl::value_t const & from ) {
						assert( to );
						if( from.is_null( ) ) {
							to.reset( );
						} else {
							T result;
							json_to_value( result, from );
							*to = std::move( result );
						}
					}

					template<typename U = std::string>
					void json_to_value<std::string, U>( std::string & to, impl::value_t const & from ) {
						assert( !from.is_null( ) );
						to = get<U>( from );
					}

					template<typename U = bool>
					void json_to_value<bool>( bool & to, impl::value_t const & from ) {
						assert( !from.is_null( ) );
						to = get<U>( from );
					}

					template<typename U = int64_t>
					void json_to_value<int64_t>( int64_t & to, impl::value_t const & from ) {
						assert( !from.is_null( ) );
						to = get<U>( from );
					}

					template<typename U = double>
					void json_to_value<double>( double & to, impl::value_t const & from ) {
						assert( !from.is_null( ) );
						to = get<U>( from );
					}

					template<typename T, typename U = impl::array_value>
					void json_to_value<std::vector>( std::vector<T> & to, impl::value_t const & from ) {
						assert( from.is_array( ) );
						to.clear( );
						auto const & arry = from.get_array( ).items( );
						to.resize( arry.size( ) );
						for( size_t n = 0; n < to.size( ); ++n ) {
							json_to_value<U>( to[n], arry[n] );
						}
					}

					template<typename T, typename U, typename V = T, typename W = U>
					void json_to_value( T &, impl::value_t const &, boost::string_ref const, boost::string_ref const );

					template<typename Key, typename Value, typename Key_From = Key, typename Value_From = Value>
					void json_to_value<std::pair<Key, Value>>( std::pair<Key, Value> & to, impl::value_t const & from, boost::string_ref const KeyName = "key", boost::string_ref const ValueName = "value" ) {
						assert( from.is_array( ) );
						auto const & arry = from.get_array( );
						assert( arry.items.size( ) == 2 );

						std::pair<Key, Value> result;
						json_to_value <Key, Key_From>( result.first, arry.items[0] );
						json_to_value<Value, Value_From>( result.second, arry.items[1] );
						to = std::move( result );
					}

					template<typename Key, typename Value, typename Key_From = Key, typename Value_From = Value>
					void json_to_value<std::map<Key, Value>>( std::map<Key, Value> & to, impl::value_t const & from, boost::string_ref const KeyName = "key", boost::string_ref const ValueName = "value" ) {
						assert( from.is_array( ) );// we are an array of objects [ { "key" : key0, "value" : value1 }, ... { "key" : keyN, "value" : valueN } ]
						auto const & arry = from.get_array( );
						to.clear( );
						for( auto const & obj : arry.items ) {
							auto key_it = obj.find( KeyName );
							assert( key_it != obj.end( ) );
							Key key;
							json_to_value<Key_From>( key, *key_it );

							auto value_it = obj.find( ValueName );
							assert( value_it != obj.end( ) );
							Value value;
							json_to_value<Value_From>( value, *value_it );
							to.emplace( std::move( key ), std::move( value ) );
						}
					}

					template<typename Key, typename Value, typename Key_From = Key, typename Value_From = Value>
					void json_to_value<std::unordered_map<Key, Value>>( std::map<Key, Value> & to, impl::value_t const & from, boost::string_ref const KeyName = "key", boost::string_ref const ValueName = "value" ) {
						assert( from.is_array( ) );// we are an array of objects [ { "key" : key0, "value" : value1 }, ... { "key" : keyN, "value" : valueN } ]
						auto const & arry = from.get_array( );
						to.clear( );
						for( auto const & obj : arry.items ) {
							auto key_it = obj.find( KeyName );
							assert( key_it != obj.end( ) );
							Key key;
							json_to_value<Key_From>( key, *key_it );

							auto value_it = obj.find( ValueName );
							assert( value_it != obj.end( ) );
							Value value;
							json_to_value<Value_From>( value, *value_it );
							to.emplace( std::move( key ), std::move( value ) );
						}
					}

					void json_to_value( JsonLink & to, impl::value_t const & from );
				}	// namespace details

				struct JsonLink {
					enum class Action { encode, decode };
				private:
					std::string m_name;
					using encode_function_t = std::function < void( std::string & json_text ) > ;
					using decode_function_t = std::function < void( json_obj json_values ) > ;

					struct bind_functions_t {
						encode_function_t encode;
						decode_function_t decode;
					};
					std::unordered_map<std::string, bind_functions_t> m_data_map;

					template<typename T>
					JsonLink& link_value( boost::string_ref name, T& value ) {
						set_name( value, name );
						m_data_map[name.to_string( )] = standard_bind_functions( name, value );
						return *this;
					}

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
					void decode( json_obj json_values );

					void reset_jsonlink( );

					template<typename T>
					void call_decode( T &, json_obj ) { }

					void call_decode( JsonLink & obj, json_obj json_values ) {
						obj.decode( std::move( json_values ) );
					}

					template<typename T>
					void set_name( T &, boost::string_ref ) { }

					void set_name( JsonLink & obj, boost::string_ref name ) {
						obj.json_object_name( ) = name.to_string( );
					}

					template<typename T>
					encode_function_t standard_encoder( boost::string_ref name, T & value ) {
						auto value_ptr = &value;
						auto name_copy = name.to_string( );
						return[value_ptr, name_copy]( std::string & json_text ) {
							assert( value_ptr );
							json_text = value_to_json( name_copy, *value_ptr );
						};
					}

					template<typename T>
					static T decoder_helper( json_obj const & json_values, boost::string_ref name ) {
						assert( json_values );
						auto obj = json_values->get_object( );
						auto member = obj.find( name );
						if( obj.end( ) == member ) {
							// TODO: determine if correct course of action
							throw std::runtime_error( "JSON object does not match expected object layout" );
						}
						return get<T>( member->second );
					}

					template<typename T>
					static	boost::optional<T> nullable_decoder_helper( json_obj const & json_values, boost::string_ref name ) {
						assert( json_values );
						auto obj = json_values->get_object( );
						auto member = obj.find( name );
						if( obj.end( ) == member ) {
							// TODO: determine if correct course of action
							throw std::runtime_error( "JSON object does not match expected object layout" );
						}
						if( member->second.is_null( ) ) {
							return boost::optional<T>( );
						}
						return boost::optional<T>( get<T>( member->second ) );
					}

					template<typename T, typename U = T>
					encode_function_t standard_decoder( boost::string_ref name, T & value ) {
						auto value_ptr = &value;
						auto name_copy = name.to_string( );
						return[value_ptr, name_copy]( json_obj json_values ) mutable {
							assert( value_ptr );
							auto new_val = decoder_helper<U>( json_values, name_copy );
							*value_ptr = new_val;
						};
					}

					template<typename T, typename U = T>
					encode_function_t standard_decoder( boost::string_ref name, boost::optional<T> & value ) {
						auto value_ptr = &value;
						auto name_copy = name.to_string( );
						return[value_ptr, name_copy]( json_obj json_values ) mutable {
							assert( value_ptr );
							auto new_val = nullable_decoder_helper<U>( json_values, name_copy );
							*value_ptr = new_val;
						};
					}

					template<typename T>
					bind_functions_t standard_bind_functions( boost::string_ref name, T& value ) {
						bind_functions_t bind_functions;
						bind_functions.encode = standard_encoder( name, value );
						bind_functions.decode = standard_decoder( name, value );
						return bind_functions;
					}

					template<typename T>
					JsonLink& link_integral( boost::string_ref name, T& value ) {
						auto value_ptr = &value;
						set_name( value, name.to_string( ) );
						bind_functions_t bind_functions;
						bind_functions.encode = standard_encoder( name, value );

						bind_functions.decode = [value_ptr, name]( json_obj const & json_values ) mutable {
							assert( value_ptr );
							auto result = decoder_helper<int64_t>( json_values, name );
							assert( result <= std::numeric_limits<T>::max( ) );
							assert( result >= std::numeric_limits<T>::min( ) );
							*value_ptr = static_cast<T>(result);
						};
						m_data_map[name.to_string( )] = std::move( bind_functions );
						return *this;
					}

					template<typename T>
					JsonLink& link_integral( boost::string_ref name, boost::optional<T>& value ) {
						auto value_ptr = &value;
						set_name( value, name.to_string( ) );
						bind_functions_t bind_functions;
						bind_functions.encode = standard_encoder( name, value );

						bind_functions.decode = [value_ptr, name]( json_obj const & json_values ) mutable {
							assert( value_ptr );
							auto result = nullable_decoder_helper<int64_t>( json_values, name );
							if( result ) {
								assert( result <= std::numeric_limits<T>::max( ) );
								assert( result >= std::numeric_limits<T>::min( ) );
							}
							*value_ptr = static_cast<T>(result);
						};
						m_data_map[name.to_string( )] = std::move( bind_functions );
						return *this;
					}

					template<typename T>
					JsonLink& link_real( boost::string_ref name, T& value ) {
						auto value_ptr = &value;
						set_name( value, name.to_string( ) );
						bind_functions_t bind_functions;
						bind_functions.encode = standard_encoder( name, value );
						bind_functions.decode = standard_decoder<double>( name, value );
						m_data_map[name.to_string( )] = std::move( bind_functions );
						return *this;
					}

					template<typename T>
					JsonLink& link_string( boost::string_ref name, T& value ) {
						return link_value( name, value );
					}

					template<typename T>
					JsonLink& link_boolean( boost::string_ref name, T& value ) {
						return link_value( name, value );
					}

					template<typename T>
					JsonLink& link_object( boost::string_ref name, T& value ) {
						auto value_ptr = &value;
						set_name( value, name.to_string( ) );
						bind_functions_t bind_functions;
						bind_functions.encode = standard_encoder( name, value );
						bind_functions.decode = [value_ptr, name]( json_obj const & json_values ) mutable {
							assert( value_ptr );
							assert( json_values );
							auto obj = json_values->get_object( );
							auto member = obj.find( name );
							if( obj.end( ) == member ) {
								// TODO: determine if correct course of action
								throw std::runtime_error( "JSON object does not match expected object layout" );
							}
							assert( !member->second.is_null( ) );
							value_ptr->decode( std::make_shared<impl::value_t>( member->second ) );
						};
						m_data_map[name.to_string( )] = std::move( bind_functions );
						return *this;
					}

					template<typename T>
					JsonLink& link_object( boost::string_ref name, boost::optional<T>& value ) {
						auto value_ptr = &value;
						set_name( value, name.to_string( ) );
						bind_functions_t bind_functions;
						bind_functions.encode = standard_encoder( name, value );
						bind_functions.decode = [value_ptr, name]( json_obj const & json_values ) mutable {
							assert( value_ptr );
							assert( json_values );
							auto obj = json_values->get_object( );
							auto member = obj.find( name );
							if( obj.end( ) == member ) {
								// TODO: determine if correct course of action
								throw std::runtime_error( "JSON object does not match expected object layout" );
							}
							if( member->second.is_null( ) ) {
								*value_ptr = boost::optional<T>( );
							} else {
								(*value_ptr)->decode( std::make_shared<impl::value_t>( member->second ) );
							}
						};
						m_data_map[name.to_string( )] = std::move( bind_functions );
						return *this;
					}

					template<typename T>
					JsonLink& link_array( boost::string_ref name, T& value ) {
						auto value_ptr = &value;
						set_name( value, name.to_string( ) );
						bind_functions_t bind_functions;
						bind_functions.encode = standard_encoder( name, value );
						bind_functions.decode = [value_ptr, name]( json_obj const & json_values ) mutable {
							assert( value_ptr );
							assert( json_values );
							auto obj = json_values->get_object( );
							auto member = obj.find( name );
							if( obj.end( ) == member ) {
								// TODO: determine if correct course of action
								throw std::runtime_error( "JSON object does not match expected object layout" );
							}
							assert( !member->second.is_null( ) );
							auto container = *value_ptr;
							container.clear( );
							for( auto & item : member->second.get_array( ).items ) {
								typename T::value_type result;
								json_to_value( result, item );
								container.push_back( std::move( result ) );
							}
						};
						m_data_map[name.to_string( )] = std::move( bind_functions );
						return *this;
					}

					template<typename T>
					JsonLink& link_array( boost::string_ref name, boost::optional<T>& value ) {
						auto value_ptr = &value;
						set_name( value, name.to_string( ) );
						bind_functions_t bind_functions;
						bind_functions.encode = standard_encoder( name, value );
						bind_functions.decode = [value_ptr, name]( json_obj const & json_values ) mutable {
							assert( value_ptr );
							assert( json_values );
							auto obj = json_values->get_object( );
							auto member = obj.find( name );
							if( obj.end( ) == member ) {
								// TODO: determine if correct course of action
								throw std::runtime_error( "JSON object does not match expected object layout" );
							}
							if( member->second.is_null( ) ) {
								*value_ptr = boost::optional<T>( );
							} else {
								auto container = *(*value_ptr);
								container.clear( );
								for( auto & item : member->second.get_array( ).items ) {
									typename T::value_type result;
									details::json_to_value( result, item );
									container.push_back( std::move( result ) );
								}
							}
						};
						m_data_map[name.to_string( )] = std::move( bind_functions );
						return *this;
					}

					template<typename T>
					JsonLink& link_map( boost::string_ref name, T& value ) {
						auto value_ptr = &value;
						set_name( value, name.to_string( ) );
						bind_functions_t bind_functions;
						bind_functions.encode = standard_encoder( name, value );
						bind_functions.decode = [value_ptr, name]( json_obj const & json_values ) mutable {
							assert( value_ptr );
							assert( json_values );
							auto val_obj = json_values->get_object( );
							auto member = val_obj.find( name );
							if( val_obj.end( ) == member ) {
								// TODO: determine if correct course of action
								throw std::runtime_error( "JSON object does not match expected object layout" );
							}
							auto container = *value_ptr;
							container.clear( );
							assert( !member->second.is_null( ) );
							for( auto const & item : member->second.get_array( ).items ) {
								auto const & obj = item.get_object( );
								auto key = obj.find( "key" )->second.get_string( );
								auto val = obj.find( "value" )->second;
								typename T::mapped_type result;
								details::json_to_value( result, val );
								container[key] = std::move( result );
							}
						};
						m_data_map[name.to_string( )] = std::move( bind_functions );
						return *this;
					}

					template<typename T>
					JsonLink& link_map( boost::string_ref name, boost::optional<T>& value ) {
						auto value_ptr = &value;
						set_name( value, name.to_string( ) );
						bind_functions_t bind_functions;
						bind_functions.encode = standard_encoder( name, value );
						bind_functions.decode = [value_ptr, name]( json_obj const & json_values ) mutable {
							assert( value_ptr );
							assert( json_values );
							auto val_obj = json_values->get_object( );
							auto member = val_obj.find( name );
							if( val_obj.end( ) == member ) {
								// TODO: determine if correct course of action
								throw std::runtime_error( "JSON object does not match expected object layout" );
							} else {
								if( member->second.is_null( ) ) {
									*value_ptr = boost::optional<T>( );
								} else {
									auto container = *(*value_ptr);
									container.clear( );
									for( auto const & item : member->second.get_array( ).items ) {
										auto const & obj = item.get_object( );
										auto key = obj.find( "key" )->second.get_string( );
										auto val = obj.find( "value" )->second;
										typename T::mapped_type result;
										details::json_to_value( result, val );
										container[key] = std::move( result );
									}
								}
							}
						};
						m_data_map[name.to_string( )] = std::move( bind_functions );
						return *this;
					}

					template<typename T, typename U = std::string>
					JsonLink& link_streamable( boost::string_ref name, T& value ) {
						auto value_ptr = &value;
						set_name( value, name.to_string( ) );
						bind_functions_t bind_functions;
						bind_functions.encode = standard_encoder( name, value );
						bind_functions.decode = [value_ptr, name]( json_obj const & json_values ) mutable {
							assert( value_ptr );
							assert( json_values );
							auto obj = json_values->get_object( );
							auto member = obj.find( name );
							if( obj.end( ) == member ) {
								// TODO: determine if correct course of action
								throw std::runtime_error( "JSON object does not match expected object layout" );
							}
							assert( !member->second.is_null( ) );
							assert( member->second.is_string( ) );
							*value_ptr = boost::lexical_cast<T>(get<U>( member->second ));
						};
						m_data_map[name.to_string( )] = std::move( bind_functions );
						return *this;
					}

					//JsonLink& link_timestamp( std::string name, std::time_t& value );
				};	// struct JsonLink

				std::string value_to_json( std::string const & name, JsonLink const & obj );

				std::ostream& operator<<(std::ostream& os, daw::nodepp::base::json::JsonLink const & data);

				template<typename T, typename EnableIf = decltype(std::declval<T>( ).serialize_to_json( ))>
				std::ostream& operator<<(std::ostream& os, T const & data) {
					os << data.serialize_to_json( );
				}
			}	// namespace json
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
