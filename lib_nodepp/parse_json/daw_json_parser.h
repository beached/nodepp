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
#include <boost/utility/string_ref.hpp>
#include <boost/variant.hpp>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "daw_common_mixins.h"

namespace daw {
	namespace json {
		namespace impl {
			struct null_value final { };
			struct object_value;
			struct array_value;

			class value_t {
				union {
					int64_t integral;
					double real;
					std::string* string;
					bool boolean;
					std::nullptr_t null;
					array_value* array;
					object_value* object;
				} m_value;
			public:
				enum class value_types { integral, real, string, boolean, null, array, object };
			private:
				value_types m_value_type;
			public:
				value_t( );
				explicit value_t( int64_t const & value );
				explicit value_t( double const & value );
				explicit value_t( std::string value );
				explicit value_t( boost::string_ref value );
				explicit value_t( bool value );
				explicit value_t( std::nullptr_t value );
				explicit value_t( array_value value );
				explicit value_t( object_value value );
				~value_t( );
				value_t( value_t const & other );
				value_t & operator=(value_t const & rhs);
				value_t( value_t && );
				value_t & operator=(value_t &&);
				int64_t const & get_integral( ) const;
				int64_t & get_integral( );
				double const & get_real( ) const;
				double & get_real( );
				std::string const & get_string( ) const;
				std::string & get_string( );
				bool const & get_boolean( ) const;
				bool & get_boolean( );
				object_value const & get_object( ) const;
				object_value & get_object( );
				array_value const & get_array( ) const;
				array_value & get_array( );
				value_types type( ) const;
				void cleanup( );

				bool is_integral( ) const;
				bool is_real( ) const;
				bool is_string( ) const;
				bool is_boolean( ) const;
				bool is_null( ) const;
				bool is_array( ) const;
				bool is_object( ) const;
			};

			using value_opt_t = boost::optional < value_t > ;

			struct array_value final {
				std::vector<value_t> items;
			};

			using object_value_item = std::pair < std::string, value_t > ;

			object_value_item make_object_value_item( std::string first, value_t second );

			struct object_value final: public daw::mixins::VectorLikeProxy < object_value, std::vector<object_value_item> > {
				std::vector<object_value_item> members;
				inline std::vector<object_value_item> & container( ) {
					return members;
				}

				inline std::vector<object_value_item> const & container( ) const {
					return members;
				}

				// 				using iterator = std::vector<object_value_item>::iterator;
				// 				using const_iterator = std::vector<object_value_item>::const_iterator;
				// 				using value_type = object_value_item;
				using key_type = std::string;
				using mapped_type = value_t;

				iterator find( boost::string_ref const key );
				const_iterator find( boost::string_ref const key ) const;

				// 				iterator begin( );
				// 				const_iterator begin( ) const;
				// 				iterator end( );
				// 				const_iterator end( ) const;

				// 				iterator insert( iterator where, object_value_item item );
				// 				void push_back( object_value_item item );
				mapped_type & operator[]( boost::string_ref key );
				mapped_type const & operator[]( boost::string_ref key ) const;
			};
			std::ostream& operator<<(std::ostream& os, value_t const & value);
			std::ostream& operator<<(std::ostream& os, std::shared_ptr<value_t> const & value);
		}	// namespace impl
		using json_obj = std::shared_ptr < impl::value_t > ;

		json_obj parse_json( boost::string_ref const json_text );

		template<typename T>
		T const & get( impl::value_t const & );/*
											   static_assert(false, "Unsupported get type called");
											   }*/

		template<> int64_t const & get<int64_t>( impl::value_t const & val );
		template<> double const & get<double>( impl::value_t const & val );
		template<> std::string const & get<std::string>( impl::value_t const & val );
		template<> bool const & get<bool>( impl::value_t const & val );
		template<> impl::object_value const & get<impl::object_value>( impl::value_t const & val );
		template<> impl::array_value const & get<impl::array_value>( impl::value_t const & val );
	}	// namespace json
}	// namespace daw
