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
#include <vector>

#include <daw/daw_common_mixins.h>
#include <daw/char_range/daw_char_range.h>

namespace daw {
	namespace json {
		struct JsonParserException final {
			std::string message;
			JsonParserException( ) = default;
			JsonParserException(std::string msg);
			~JsonParserException( ) = default;
			JsonParserException( JsonParserException const & ) = default;
			JsonParserException( JsonParserException && ) = default;
			JsonParserException & operator=( JsonParserException const & ) = default;
			JsonParserException & operator=( JsonParserException && ) = default;
		};	// struct JsonParserException

		namespace impl {
			class value_t;
			struct null_value final { };

			using string_value = range::CharRange;

			using object_value_item = std::pair <string_value, value_t>;

			object_value_item make_object_value_item( string_value first, value_t second );

			struct object_value: public daw::mixins::VectorLikeProxy <object_value, std::vector<object_value_item>> {
				std::vector<object_value_item> members_v;
				object_value( ) = default;
				~object_value( ) = default;

				object_value( object_value const & ) = default;
				object_value( object_value && ) = default;

				object_value & operator=( object_value const & ) = default;
				object_value & operator=( object_value && ) = default;


				inline std::vector<object_value_item> & container( ) {
					return members_v;
				}

				inline std::vector<object_value_item> const & container( ) const {
					return members_v;
				}

				using key_type = std::string;
				using mapped_type = value_t;

				iterator find( boost::string_ref const key );
				const_iterator find( boost::string_ref const key ) const;

				mapped_type & operator[]( boost::string_ref key );
				mapped_type const & operator[]( boost::string_ref key ) const;
				
				inline void shrink_to_fit( ) {
					members_v.shrink_to_fit( );
				}
			};	// struct object_value

			using array_value = std::vector<value_t>;

			class value_t {
				struct u_value_t {
					union {
						int64_t integral;
						double real;
						string_value* string;
						bool boolean;
						array_value* array_v;
						object_value* object;
					};
					
					void clear( );
				} m_value;
			public:
				enum class value_types { integral, real, string, boolean, null, array, object };
			private:
				value_types m_value_type;
			public:
				value_t( );
				explicit value_t( int64_t const & value );
				explicit value_t( double const & value );
				explicit value_t( std::string const & value );
				explicit value_t( boost::string_ref value );
				explicit value_t( string_value value );
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
				std::string get_string( ) const;
				string_value get_string_value( ) const;
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
			};	// class value_t

			using value_opt_t = boost::optional <value_t> ;

			std::string to_string( value_t const & value );
			std::string to_string( std::ostream& os, std::shared_ptr<value_t> const & value );

			std::ostream& operator<<( std::ostream& os, value_t const & value );
			std::ostream& operator<<( std::ostream& os, std::shared_ptr<value_t> const & value );
		}	// namespace impl
		using json_obj = impl::value_t;

		json_obj parse_json( char const * Begin, char const * End );
		json_obj parse_json( boost::string_ref const json_text );

		template<typename T>
		T get( impl::value_t const & );/*
											   static_assert(false, "Unsupported get type called");
											   }*/

		template<> int64_t get<int64_t>( impl::value_t const & val );
		template<> double get<double>( impl::value_t const & val );
		template<> std::string get<std::string>( impl::value_t const & val );
		template<> bool get<bool>( impl::value_t const & val );
		template<> impl::object_value get<impl::object_value>( impl::value_t const & val );
		template<> impl::array_value get<impl::array_value>( impl::value_t const & val );
	}	// namespace json
}	// namespace daw
