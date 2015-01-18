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
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "daw_json_parser.h"
#include "daw_range.h"
#include "daw_traits.h"

namespace daw {
	namespace json {
		namespace details {
			std::string json_name( boost::string_ref name );
			std::string enbrace( boost::string_ref json_value );
		}

		//		std::string to_string( boost::string_ref value );

		std::string enquote( boost::string_ref value );

		namespace generate {
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

			// Integral Numbers
			template<typename Number, typename std::enable_if<std::is_integral<Number>::value, int>::type = 0>
			std::string value_to_json_number( std::string const & name, Number const & value ) {
				return daw::json::details::json_name( name ) + std::to_string( value );
			}

			// Real Numbers
			template<typename Number, typename std::enable_if<std::is_floating_point<Number>::value, int>::type = 0>
			std::string value_to_json_number( std::string const & name, Number const & value ) {
				std::stringstream ss;
				ss << daw::json::details::json_name( name );
				ss << std::setprecision( std::numeric_limits<Number>::max_digits10 ) << value;
				return ss.str( );
			}

			std::string value_to_json( std::string const & name, int const & value );

			std::string value_to_json( std::string const & name, unsigned int const & value );

			std::string value_to_json( std::string const & name, int64_t const & value );

			std::string value_to_json( std::string const & name, uint64_t const & value );

			std::string value_to_json( std::string const & name, double const & value );

			template<typename T> void value_to_json( std::string const & name, std::shared_ptr<T> const & value );
			template<typename T> void value_to_json( std::string const & name, std::weak_ptr<T> const & value );
			template<typename Optional>
			std::string value_to_json( std::string const & name, boost::optional<Optional> const & value );

			template<typename First, typename Second>
			std::string value_to_json( boost::string_ref name, std::pair<First, Second> const & value );

			// container/array.
			template<typename Container, typename std::enable_if<daw::traits::is_container_not_string<Container>::value, long>::type = 0>
			std::string value_to_json( std::string const & name, Container const & values ) {
				std::stringstream result;
				result << daw::json::details::json_name( name ) + "[ ";
				{
					auto values_range = daw::range::make_range( values.begin( ), values.end( ) );
					if( !at_end( values_range ) ) {
						result << value_to_json( "", values_range.front( ) );
						values_range.move_next( );
						for( auto const & item : values_range ) {
							result << "," << value_to_json( "", item );
						}
					}
				}
				result << " ]";
				return result.str( );
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
					return value_to_json( name, *value );
				} else {
					return value_to_json( name );
				}
			}

			// pair types
			template<typename First, typename Second>
			std::string value_to_json( boost::string_ref name, std::pair<First, Second> const & value ) {
				return daw::json::details::json_name( name.to_string( ) ) + "{ " + value_to_json( "key", value.first ) + ", " + value_to_json( "value", value.second ) + " }";
			}
		}	// namespace generate

		namespace details {
			void json_to_value( std::string & to, impl::value_t const & from );
			void json_to_value( bool & to, impl::value_t const & from );
			void json_to_value( int64_t & to, impl::value_t const & from );
			void json_to_value( double & to, impl::value_t const & from );
			void json_to_value( float & to, impl::value_t const & from );
			template<typename T> void json_to_value( std::shared_ptr<T> & to, impl::value_t const & from );
			template<typename T> void json_to_value( std::vector<T> & to, impl::value_t const & from );
			template<typename T>
			void json_to_value( boost::optional<T> & to, impl::value_t const & from );

			template<typename Key, typename Value>
			void json_to_value( std::pair<Key, Value> & to, impl::value_t const & from, boost::string_ref const KeyName = "key", boost::string_ref const ValueName = "value" );

			template<typename MapLike, typename std::enable_if<daw::traits::is_map_type<MapLike>::value, long>::type = 0>
			void json_to_value( MapLike & to, impl::value_t const & from, boost::string_ref const KeyName = "key", boost::string_ref const ValueName = "value" );

			// Number, other integral
			template<typename T, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, int64_t>::value, long>::type = 0>
			void json_to_value( T & to, impl::value_t const & from ) {
				assert( from.is_integral( ) );
				auto result = get<int64_t>( from );
				assert( static_cast<int64_t>(std::numeric_limits<T>::max( )) >= result );
				assert( static_cast<int64_t>(std::numeric_limits<T>::min( )) <= result );
				to = static_cast<T>(result);
			}

			// A nullable json value with a result of boost::optional
			template<typename T>
			void json_to_value( boost::optional<T> & to, impl::value_t const & from ) {
				if( from.is_null( ) ) {
					to.reset( );
				} else {
					T result;
					json_to_value( result, from );
					*to = std::move( result );
				}
			}

			// A nullable json value with a result of std::shared_ptr
			template<typename T>
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

			// Array
			template<typename T>
			void json_to_value( std::vector<T> & to, impl::value_t const & from ) {
				assert( from.is_array( ) );
				auto const & source_array = from.get_array( ).items;
				to.resize( source_array.size( ) );

				std::transform( std::begin( source_array ), std::end( source_array ), std::begin( to ), []( impl::value_t const & value_in ) {
					T tmp;
					json_to_value( tmp, value_in );
					return tmp;
				} );
			}

			template<typename Key, typename Value>
			void json_to_value( std::pair<Key, Value> & to, impl::value_t const & from, boost::string_ref const KeyName, boost::string_ref const ValueName ) {
				assert( from.is_array( ) );

				auto const & arry = from.get_array( );
				assert( arry.items.size( ) == 2 );

				std::pair<Key, Value> result;
				json_to_value( result.first, arry.items[0] );
				json_to_value( result.second, arry.items[1] );
				to = std::move( result );
			}

			template<typename MapLike, typename std::enable_if<daw::traits::is_map_type<MapLike>::value, long>::type>
			void json_to_value( MapLike & to, impl::value_t const & from, boost::string_ref const KeyName, boost::string_ref const ValueName ) {
				assert( from.is_array( ) );	// we are an array of objects like [ { "key" : key0, "value" : value1 }, ... { "key" : keyN, "value" : valueN } ]
				using Key = typename std::remove_cv<typename MapLike::key_type>::type;
				using Value = typename std::remove_cv<typename MapLike::mapped_type>::type;
				auto const & source_array = from.get_array( ).items;
				to.clear( );
				using param_t = typename std::iterator_traits<decltype(std::begin( source_array ))>::value_type;
				std::transform( std::begin( source_array ), std::end( source_array ), std::begin( to ), []( param_t const & kv_obj ) {
					typename MapLike::value_type result;
					json_to_value( result, kv_obj );
					return result;
				} );
			}

			template<typename T, typename EnableIf = decltype(std::declval<T>( ).serialize_to_json( ))>
			std::ostream& operator<<(std::ostream& os, T const & data) {
				os << data.serialize_to_json( );
			}

			template<typename T>
			T decode_to_new( std::string const &json_values ) {
				T result;
				result.decode( json_values );
				return result;
			}
		}	// namespace details
	}	// namespace json
}	// namespace daw
