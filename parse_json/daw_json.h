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
			template<typename T> void json_to_value( boost::optional<T> & to, impl::value_t const & from );
			template<typename Key, typename Value> void json_to_value( std::pair<Key, Value> & to, impl::value_t const & from );

			template<typename Container, typename std::enable_if<daw::traits::is_container_not_string<Container>::value, long>::type = 0>
			void json_to_value( Container & to, impl::value_t const & from );

			// Number, other integral
			template<typename T, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, int64_t>::value, long>::type = 0>
			void json_to_value( T & to, impl::value_t const & from ) {
				static_assert(!std::is_const<decltype(to)>::value, "To parameter on json_to_value cannot be const");
				assert( from.is_integral( ) );
				auto result = get<int64_t>( from );
				assert( static_cast<int64_t>(std::numeric_limits<T>::max( )) >= result );
				assert( static_cast<int64_t>(std::numeric_limits<T>::min( )) <= result );
				to = static_cast<T>(result);
			}

			// A nullable json value with a result of boost::optional
			template<typename T>
			void json_to_value( boost::optional<T> & to, impl::value_t const & from ) {
				static_assert(!std::is_const<decltype(to)>::value, "To parameter on json_to_value cannot be const");
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
				static_assert(!std::is_const<decltype(to)>::value, "To parameter on json_to_value cannot be const");
				assert( to );
				if( from.is_null( ) ) {
					to.reset( );
				} else {
					T result;
					json_to_value( result, from );
					*to = std::move( result );
				}
			}

			template<typename Key, typename Value>
			void json_to_value( std::pair<Key, Value> & to, impl::value_t const & from ) {
				static_assert(!std::is_const<decltype(to)>::value, "To parameter on json_to_value cannot be const");
				assert( from.is_array( ) );

				auto const & arry = from.get_array( );
				assert( arry.items.size( ) == 2 );

				Key key;
				auto const & key_obj = arry.items[0].get_object( )["key"];
				json_to_value( key, key_obj );
				Value value;
				auto const & value_obj = arry.items[0].get_object( )["value"];
				json_to_value( value, value_obj );
				to = std::make_pair<K, V>( std::move( key ), std::move( value ) );
			}

			// Containers, but not string or map like.  Map like have a const key and that makes not
			// doing bad things like const cast difficult.  So has begin/end/value_type but not substr
			template<typename Container, typename std::enable_if<daw::traits::is_container_not_string_or_map_like<Container>::value, long>::type>
			void json_to_value( Container & to, impl::value_t const & from ) {
				static_assert(!std::is_const<Container>::value, "To parameter on json_to_value cannot be const");
				assert( from.is_array( ) );
				auto const & source_array = from.get_array( ).items;
				to.clear( );

				for( auto const & source_value : source_array ) {
					typename Container::value_type tmp;
					json_to_value( tmp, source_value );
					to.insert( std::end( to ), std::move( tmp ) );
				}
			}

			template<typename Container, typename std::enable_if<daw::traits::is_map_like<Container>::value, long>::type>
			void json_to_value( Container & to, impl::value_t const & from ) {
				static_assert(!std::is_const<Container>::value, "To parameter on json_to_value cannot be const");
				assert( from.is_array( ) );
				auto const & source_array = from.get_array( ).items;
				to.clear( );

				for( auto const & source_value : source_array ) {
					using key_t = typename std::decay<Container::key_type>::type;
					using value_t = typename std::decay<Container::mapped_type>::type;
					std::pair<key_t, value_t> tmp;
					json_to_value( tmp, source_value );
					to.insert( std::end( to ), std::move( tmp ) );
				}
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
