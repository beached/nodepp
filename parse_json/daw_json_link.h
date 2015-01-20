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
#include <boost/utility/string_ref.hpp>
#include <functional>
#include <limits>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "daw_json.h"
#include "daw_json_parser.h"

namespace daw {
	namespace json {
		template<typename Derived> class JsonLink;

		template<typename Derived> std::ostream& operator<<(std::ostream& os, JsonLink<Derived> const & data);
		template<typename Derived> void json_to_value( JsonLink<Derived> & to, impl::value_t const & from );
		template<typename Derived> std::string value_to_json( boost::string_ref name, JsonLink<Derived> const & obj );
		template<typename Derived> ::daw::json::impl::value_t get_schema( boost::string_ref name, JsonLink<Derived> const & obj );

		namespace schema {
			::daw::json::impl::value_t get_schema( boost::string_ref name );
			::daw::json::impl::value_t get_schema( boost::string_ref name, bool const & );
			::daw::json::impl::value_t get_schema( boost::string_ref name, std::nullptr_t );
			::daw::json::impl::value_t get_schema( boost::string_ref name, std::string const & );
			::daw::json::impl::value_t make_type_obj( boost::string_ref name, ::daw::json::impl::value_t selected_type );

			template<typename Key, typename Value>
			::daw::json::impl::value_t get_schema( boost::string_ref name, std::pair<Key, Value> const & );

			template<typename T, typename std::enable_if<daw::traits::is_container_not_string<T>::value, long>::type = 0>
			::daw::json::impl::value_t get_schema( boost::string_ref name, T const & );

			template<typename T, typename std::enable_if<std::is_floating_point<T>::value, long>::type = 0>
			::daw::json::impl::value_t get_schema( boost::string_ref name, T const & );

			template<typename T, typename std::enable_if<std::is_integral<T>::value, long>::type = 0>
			::daw::json::impl::value_t get_schema( boost::string_ref name, T const & );

			template<typename T>
			::daw::json::impl::value_t get_schema( boost::string_ref name, boost::optional<T> const & );

			template<typename T, typename std::enable_if<daw::traits::is_streamable<T>::value, long>::type = 0>
			::daw::json::impl::value_t get_schema( boost::string_ref name, T const & );

			template<typename Key, typename Value>
			::daw::json::impl::value_t get_schema( boost::string_ref name, std::pair<Key, Value> const & ) {
				using ::daw::json::impl::make_object_value_item;

				::daw::json::impl::object_value result;
				using key_t = typename std::decay<Key>::type;
				using value_t = typename std::decay<Value>::type;
				key_t k;
				value_t v;
				result.push_back( make_object_value_item( "key", get_schema( "key", k ) ) );
				result.push_back( make_object_value_item( "value", get_schema( "value", v ) ) );
				return make_type_obj( name, std::move( ::daw::json::impl::value_t( std::move( result ) ) ) );
			}

			template<typename T, typename std::enable_if<daw::traits::is_container_not_string<T>::value, long>::type>
			::daw::json::impl::value_t get_schema( boost::string_ref name, T const & ) {
				using ::daw::json::impl::make_object_value_item;
				daw::json::impl::object_value result;

				static daw::json::impl::object_value_item const obj_type { "type", daw::json::impl::value_t( daw::traits::is_map_like<T>::value ? "map" : "array" ) };
				result.push_back( obj_type );
				typename T::value_type t;
				result.push_back( make_object_value_item( "element_type", get_schema( "", t ) ) );
				return make_type_obj( name, ::daw::json::impl::value_t( std::move( result ) ) );
			}

			template<typename T, typename std::enable_if<std::is_floating_point<T>::value, long>::type>
			::daw::json::impl::value_t get_schema( boost::string_ref name, T const & ) {
				return make_type_obj( name, ::daw::json::impl::value_t( std::string( "real" ) ) );
			}

			template<typename T, typename std::enable_if<std::is_integral<T>::value, long>::type>
			::daw::json::impl::value_t get_schema( boost::string_ref name, T const & ) {
				return make_type_obj( name, ::daw::json::impl::value_t( std::string( "integer" ) ) );
			}

			template<typename T>
			::daw::json::impl::value_t get_schema( boost::string_ref name, boost::optional<T> const & ) {
				T t;
				auto result = get_schema( name, t );
				auto & obj = result.get_object( );
				obj.push_back( make_object_value_item( "nullable", ::daw::json::impl::value_t( std::string( "nullable" ) ) ) );
				return result;
			}

			template<typename T, typename std::enable_if<daw::traits::is_streamable<T>::value, long>::type>
			::daw::json::impl::value_t get_schema( boost::string_ref name, T const & ) {
				auto result = make_type_obj( name, ::daw::json::impl::value_t( std::string( "string" ) ) );
				auto & obj = result.get_object( );
				obj.push_back( make_object_value_item( "string_object", ::daw::json::impl::value_t( std::string( "string_object" ) ) ) );
				return result;
			}
		}

		template<typename Derived>
		class JsonLink {
			std::string m_name;
			using encode_function_t = std::function < void( std::string & json_text ) > ;
			using decode_function_t = std::function < void( json_obj json_values ) > ;

			struct bind_functions_t {
				encode_function_t encode;
				decode_function_t decode;
				bind_functions_t( ) : endcode( nullptr ), decode( nulptr ) { }
			};

			struct data_description_t {
				::daw::json::impl::value_t json_type;
				bind_functions_t bind_functions;
				data_description_t( ) : json_type( nullptr ), bind_functions( ) { }
			};	// struct data_description

			std::unordered_map<std::string, data_description_t> m_data_map;

			template<typename T>
			JsonLink& link_value( boost::string_ref name, T& value, ::daw::json::impl::value_t json_type ) {
				set_name( value, name );
				data_description_t data_description;
				data_description.json_type = std::move( json_type );
				data_description.bind_functions = standard_bind_functions( name, value );
				m_data_map[name.to_string( )] = std::move( data_description );
				return *this;
			}

		public:
			JsonLink( std::string name = "" ) :
				m_name( std::move( name ) ),
				m_data_map( ) { }

			~JsonLink( ) = default;

			JsonLink( JsonLink const & ) = default;

			JsonLink& operator=(JsonLink const &) = default;

			JsonLink( JsonLink && other ) :
				m_name( std::move( other.m_name ) ),
				m_data_map( std::move( other.m_data_map ) ) { }

			JsonLink& operator=(JsonLink && rhs) {
				if( this != &rhs ) {
					m_name = std::move( rhs.m_name );
					m_data_map = std::move( rhs.m_data_map );
				}
				return *this;
			}

			std::string & json_object_name( ) {
				return m_name;
			}

			std::string const & json_object_name( ) const {
				return m_name;
			}

			::daw::json::impl::value_t get_schema_obj( ) const {
				throw std::runtime_error( "Method not implemented" );
			}

			std::string encode( ) const {
				std::stringstream result;
				std::string tmp;
				auto is_first = true;
				for( auto const & value : m_data_map ) {
					value.second.bind_functions.encode( tmp );
					result << (!is_first ? ", " : "") << tmp;
					is_first = false;
				}
				return details::json_name( m_name ) + details::enbrace( result.str( ) );
			}

			void decode( json_obj const & json_values ) {
				for( auto & value : m_data_map ) {
					value.second.bind_functions.decode( json_values );
				}
			}

			void decode( boost::string_ref const json_text ) {
				decode( parse_json( json_text ) );
			}

			void reset_jsonlink( ) {
				m_data_map.clear( );
				m_name.clear( );
			}

			template<typename T>
			static void call_decode( T &, json_obj ) { }

			static void call_decode( JsonLink & obj, json_obj json_values ) {
				obj.decode( std::move( json_values ) );
			}

			template<typename T>
			static void set_name( T &, boost::string_ref ) { }

			static void set_name( JsonLink & obj, boost::string_ref name ) {
				obj.json_object_name( ) = name.to_string( );
			}

			template<typename T>
			static encode_function_t standard_encoder( boost::string_ref name, T const & value ) {
				auto value_ptr = &value;
				auto name_copy = name.to_string( );
				return[value_ptr, name_copy]( std::string & json_text ) {
					assert( value_ptr );
					using namespace generate;
					json_text = value_to_json( name_copy, *value_ptr );
				};
			}

			template<typename T>
			static T decoder_helper( boost::string_ref name, json_obj const & json_values ) {
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
			static boost::optional<T> nullable_decoder_helper( boost::string_ref name, json_obj const & json_values ) {
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
			static decode_function_t standard_decoder( boost::string_ref name, T & value ) {
				auto value_ptr = &value;
				auto name_copy = name.to_string( );
				return[value_ptr, name_copy]( json_obj json_values ) mutable {
					assert( value_ptr );
					auto new_val = decoder_helper<U>( name_copy, json_values );
					*value_ptr = new_val;
				};
			}

			template<typename T, typename U = T>
			static decode_function_t standard_decoder( boost::string_ref name, boost::optional<T> & value ) {
				auto value_ptr = &value;
				auto name_copy = name.to_string( );
				return[value_ptr, name_copy]( json_obj json_values ) mutable {
					assert( value_ptr );
					auto new_val = nullable_decoder_helper<U>( name_copy, json_values );
					*value_ptr = new_val;
				};
			}

			template<typename T>
			static bind_functions_t standard_bind_functions( boost::string_ref name, T& value ) {
				bind_functions_t bind_functions;
				bind_functions.encode = standard_encoder( name, value );
				bind_functions.decode = standard_decoder( name, value );
				return bind_functions;
			}

			template<typename T>
			JsonLink& link_integral( boost::string_ref name, T& value ) {
				auto value_ptr = &value;
				set_name( value, name.to_string( ) );
				data_description_t data_description;
				data_description.json_type = ::daw::json::impl::value_t( 0 );

				data_description.bind_functions.encode = standard_encoder( name, value );

				data_description.bind_functions.decode = [value_ptr, name]( json_obj const & json_values ) mutable {
					assert( value_ptr );
					auto result = decoder_helper<int64_t>( name, json_values );
					assert( result <= std::numeric_limits<T>::max( ) );
					assert( result >= std::numeric_limits<T>::min( ) );
					*value_ptr = static_cast<T>(result);
				};
				m_data_map[name.to_string( )] = std::move( data_description );
				return *this;
			}

			template<typename T>
			JsonLink& link_integral( boost::string_ref name, boost::optional<T>& value ) {
				auto value_ptr = &value;
				set_name( value, name.to_string( ) );
				data_description_t data_description;
				data_description.json_type = ::daw::json::impl::value_t( 0 );
				data_description.bind_functions.encode = standard_encoder( name, value );

				data_description.bind_functions.decode = [value_ptr, name]( json_obj const & json_values ) mutable {
					assert( value_ptr );
					auto result = nullable_decoder_helper<int64_t>( name, json_values );
					if( result ) {
						assert( result <= std::numeric_limits<T>::max( ) );
						assert( result >= std::numeric_limits<T>::min( ) );
					}
					*value_ptr = static_cast<T>(result);
				};
				m_data_map[name.to_string( )] = std::move( data_description );
				return *this;
			}

			template<typename T>
			JsonLink& link_real( boost::string_ref name, T& value ) {
				auto value_ptr = &value;
				set_name( value, name.to_string( ) );
				data_description_t data_description;
				data_description.json_type = ::daw::json::impl::value_t( 0.0 );
				data_description.bind_functions.encode = standard_encoder( name, value );
				data_description.bind_functions.decode = standard_decoder<double>( name, value );
				m_data_map[name.to_string( )] = std::move( data_description );
				return *this;
			}

			template<typename T>
			JsonLink& link_string( boost::string_ref name, T& value ) {
				return link_value( name, value, ::daw::json::impl::value_t( std::string( "" ) ) );
			}

			template<typename T>
			JsonLink& link_boolean( boost::string_ref name, T& value ) {
				return link_value( name, value, ::daw::json::impl::value_t( false ) );
			}

			template<typename T>
			JsonLink& link_object( boost::string_ref name, T& value ) {
				auto value_ptr = &value;
				set_name( value, name.to_string( ) );
				data_description_t data_description;
				data_description.json_type = value.get_schema_obj( );
				data_description.bind_functions.encode = standard_encoder( name, value );
				data_description.bind_functions.decode = [value_ptr, name]( json_obj const & json_values ) mutable {
					assert( value_ptr );
					assert( json_values );
					auto obj = json_values->get_object( );
					auto member = obj.find( name );
					if( obj.end( ) == member ) {
						// TODO: determine if correct course of action
						throw std::runtime_error( "JSON object does not match expected object layout" );
					}
					assert( member->second.is_object( ) );
					value_ptr->decode( std::make_shared<impl::value_t>( member->second ) );
				};
				m_data_map[name.to_string( )] = std::move( data_description );
				return *this;
			}

			template<typename T>
			JsonLink& link_object( boost::string_ref name, boost::optional<T>& value ) {
				auto value_ptr = &value;
				set_name( value, name.to_string( ) );
				data_description_t data_description;
				data_description.json_type = value ? value->get_schema_obj( ) : (T { }).get_schema( );
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
				data_description_t data_description;
				data_description.json_type = ::daw::json::impl::value_t( )
					data_description.bind_functions.encode = standard_encoder( name, value );
				data_description.bind_functions.decode = [value_ptr, name]( json_obj const & json_values ) mutable {
					assert( value_ptr );
					assert( json_values );
					auto obj = json_values->get_object( );
					auto member = obj.find( name );
					if( obj.end( ) == member ) {
						// TODO: determine if correct course of action
						throw std::runtime_error( "JSON object does not match expected object layout" );
					}
					assert( member->second.is_array( ) );
					using namespace parse;
					json_to_value( *value_ptr, member->second );
				};
				m_data_map[name.to_string( )] = std::move( data_description );
				return *this;
			}

			template<typename T>
			JsonLink& link_array( boost::string_ref name, boost::optional<T>& value ) {
				auto value_ptr = &value;
				set_name( value, name.to_string( ) );
				data_description_t data_description;
				using ::daw::json::schema::get_schema;
				data_description.json_type = get_schema( name, value );
				data_description.bind_functions.encode = standard_encoder( name, value );
				data_description.bind_functions.decode = [value_ptr, name]( json_obj const & json_values ) mutable {
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
						assert( member->second.is_array( ) );
						using namespace parse;
						json_to_value( *value_ptr, member->second );
					}
				};
				m_data_map[name.to_string( )] = std::move( data_description );
				return *this;
			}

			template<typename T>
			JsonLink& link_map( boost::string_ref name, T& value ) {
				auto value_ptr = &value;
				set_name( value, name.to_string( ) );
				data_description_t data_description;
				using ::daw::json::schema::get_schema;
				data_description.json_type = get_schema( name, value );
				data_description.bind_functions.encode = standard_encoder( name, value );
				data_description.bind_functions.decode = [value_ptr, name]( json_obj const & json_values ) mutable {
					assert( value_ptr );
					assert( json_values );
					auto val_obj = json_values->get_object( );
					auto member = val_obj.find( name );
					if( val_obj.end( ) == member ) {
						// TODO: determine if correct course of action
						throw std::runtime_error( "JSON object does not match expected object layout" );
					}
					assert( member->second.is_array( ) );
					using namespace parse;
					json_to_value( *value_ptr, member->second );
				};
				m_data_map[name.to_string( )] = std::move( data_description );
				return *this;
			}

			template<typename T>
			JsonLink& link_map( boost::string_ref name, boost::optional<T>& value ) {
				auto value_ptr = &value;
				set_name( value, name.to_string( ) );
				data_description_t data_description;
				data_description.json_type = "map?";
				data_description.bind_functions.encode = standard_encoder( name, value );
				data_description.bind_functions.decode = [value_ptr, name]( json_obj const & json_values ) mutable {
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
							assert( member->second.is_array( ) );
							using namespace parse;
							json_to_value( *value_ptr, member->second );
						}
					}
				};
				m_data_map[name.to_string( )] = std::move( data_description );
				return *this;
			}

			template<typename T>
			JsonLink& link_streamable( boost::string_ref name, T& value ) {
				auto value_ptr = &value;
				set_name( value, name );
				data_description_t data_description;
				using daw::json::schema::get_schema;
				data_description.json_type = get_schema( name, value );
				data_description.bind_functions.encode = [value_ptr, name]( std::string & json_text ) {
					assert( value_ptr );
					json_text = generate::value_to_json( name.to_string( ), boost::lexical_cast<std::string>(*value_ptr) );
				};
				data_description.bind_functions.decode = [value_ptr, name]( json_obj const & json_values ) mutable {
					assert( value_ptr );
					assert( json_values );
					auto obj = json_values->get_object( );
					auto member = obj.find( name );
					if( obj.end( ) == member ) {
						// TODO: determine if correct course of action
						throw std::runtime_error( "JSON object does not match expected object layout" );
					}
					assert( member->second.is_string( ) );
					std::stringstream ss( member->second.get_string( ) );
					auto str = ss.str( );
					ss >> *value_ptr;
				};
				m_data_map[name.to_string( )] = std::move( data_description );
				return *this;
			}

			//JsonLink& link_timestamp( std::string name, std::time_t& value );
		};	// class JsonLink

		template<typename Derived>
		std::ostream& operator<<(std::ostream& os, JsonLink<Derived> const & data) {
			os << data.encode( );
			return os;
		}

		template<typename Derived>
		void json_to_value( JsonLink<Derived> & to, impl::value_t const & from ) {
			auto val = from;
			to.decode( std::make_shared<impl::value_t>( std::move( val ) ) );
		}

		template<typename Derived>
		std::string value_to_json( boost::string_ref name, JsonLink<Derived> const & obj ) {
			return details::json_name( name ) + obj.encode( );
		}

		template<typename Derived>
		::daw::json::impl::value_t get_schema( boost::string_ref name, JsonLink<Derived> const & obj ) {
			return obj.get_schema_obj( );
		}
	}	// namespace json
}	// namespace daw
