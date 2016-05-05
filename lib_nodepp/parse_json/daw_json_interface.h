// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Darrell Wright
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
#include <boost/utility/string_ref.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

#include "daw_json_parser.h"
#include <daw/daw_traits.h>

namespace daw {
	namespace json {
		namespace details {
			std::string json_name( boost::string_ref name );
			std::string enbrace( boost::string_ref json_value );
		}

		std::string enquote( boost::string_ref value );

		namespace generate {
			//////////////////////////////////////////////////////////////////////////
			/// Summary: Convert an optionally named value to a json string

			std::string value_to_json( boost::string_ref name, std::string const & value );
			std::string value_to_json( boost::string_ref name, bool value );
			std::string value_to_json( boost::string_ref name );	// Null
			std::string value_to_json( boost::string_ref name, double const & value );
			std::string value_to_json( boost::string_ref name, int const & value );
			std::string value_to_json( boost::string_ref name, int64_t const & value );
			std::string value_to_json( boost::string_ref name, uint64_t const & value );
			std::string value_to_json( boost::string_ref name, unsigned int const & value );
			std::string value_to_json_object( boost::string_ref name, ::daw::json::impl::object_value const & object );
			std::string value_to_json( boost::string_ref name, ::daw::json::impl::value_t const & value );

			// Template value_to_json Declarations
			template<typename Container, typename std::enable_if_t<daw::traits::is_container_not_string<Container>::value, long> = 0>
			std::string value_to_json( boost::string_ref name, Container const & values );

			template<typename First, typename Second>
			std::string value_to_json( boost::string_ref name, std::pair<First, Second> const & value );

			template<typename Number, typename std::enable_if_t<std::is_floating_point<Number>::value, int> = 0>
			std::string value_to_json_number( boost::string_ref name, Number const & value );

			template<typename Number, typename std::enable_if_t<std::is_integral<Number>::value, int> = 0>
			std::string value_to_json_number( boost::string_ref name, Number const & value );

			template<typename T>
			std::string value_to_json( boost::string_ref name, boost::optional<T> const & value );

			template<typename T>
			void value_to_json( boost::string_ref name, std::shared_ptr<T> const & value );

			template<typename T>
			void value_to_json( boost::string_ref name, std::weak_ptr<T> const & value );
		}	// namespace generate

		namespace parse {
			void json_to_value( bool & to, ::daw::json::impl::value_t const & from );
			void json_to_value( double & to, ::daw::json::impl::value_t const & from );
			void json_to_value( float & to, ::daw::json::impl::value_t const & from );
			void json_to_value( int64_t & to, ::daw::json::impl::value_t const & from );
			void json_to_value( std::string & to, ::daw::json::impl::value_t const & from );

			// Template json_to_value declarations
			template<typename Container, typename std::enable_if_t<daw::traits::is_vector_like_not_string<Container>::value, long> = 0>
			void json_to_value( Container & to, ::daw::json::impl::value_t const & from );

			template<typename Key, typename Value>
			void json_to_value( std::pair<Key, Value> & to, ::daw::json::impl::value_t const & from );

			template<typename MapContainer, typename std::enable_if_t<daw::traits::is_map_like<MapContainer>::value, long> = 0>
			void json_to_value( MapContainer & to, ::daw::json::impl::value_t const & from );

			template<typename T, typename std::enable_if_t<std::is_integral<T>::value && !std::is_same<T, int64_t>::value, long> = 0>
			void json_to_value( T & to, ::daw::json::impl::value_t const & from );

			template<typename T>
			void json_to_value( boost::optional<T> & to, ::daw::json::impl::value_t const & from );

			template<typename T>
			void json_to_value( std::shared_ptr<T> & to, ::daw::json::impl::value_t const & from );

			GENERATE_HAS_MEMBER_FUNCTION_TRAIT( decode );

			template<typename T, typename std::enable_if_t<has_decode_member<T>::value, long> = 0>
			T decode_to_new( boost::string_ref json_values );
		}	// namespace parse
	}	// namespace json
}	// namespace daw

template<typename T, typename EnableIf = decltype(std::declval<T>( ).serialize_to_json( ))>
std::ostream& operator<<(std::ostream& os, T const & data) {
	os << data.serialize_to_json( );
	return os;
}
