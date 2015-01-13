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

namespace daw {
	namespace nodepp {
		namespace base {
			namespace json {
				namespace impl {
					struct null_value { };
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
						enum class value_types { integral, real, string, boolean, null, array, object };
						value_types m_value_type;
					public:
						explicit value_t( int64_t const & value );
						explicit value_t( double const & value );
						explicit value_t( std::string value );
						explicit value_t( bool value );
						explicit value_t( std::nullptr_t value );
						explicit value_t( array_value value );
						explicit value_t( object_value value );
						~value_t( );
						int64_t const & get_integral( ) const;						
						double const & get_real( ) const;
						std::string const & get_string( ) const;
						bool const & get_boolean( ) const;
						object_value const & get_object( ) const;
						array_value const & get_array( ) const;
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

					//using value_t = boost::variant < int64_t, double, std::string, bool, null_value, array_value, object_value > ;
					using value_opt_t = boost::optional < value_t > ;

					struct array_value {
						std::vector<value_t> items;
					};

					using object_value_item = std::pair < std::string, value_t > ;

					struct object_value {
						std::vector<object_value_item> members;
						using iterator = std::vector<object_value_item>::iterator;
						using const_iterator = std::vector<object_value_item>::const_iterator;
						iterator find( boost::string_ref const key );
						const_iterator find( boost::string_ref const key ) const;
						iterator begin( );
						iterator end( );
					};

				}	// namespace impl
				using json_obj = std::shared_ptr < impl::value_t > ;

				json_obj parse_json( boost::string_ref const json_text );

				template<typename T>
				T const & get( impl::value_t const & );/*
					static_assert(false, "Unsupported get type called");
				}*/

				template<> int64_t const & get<int64_t>( impl::value_t const & val ) {
					return val.get_integral( );
				}

				template<> double const & get<double>( impl::value_t const & val ) {
					return val.get_real( );
				}

				template<> std::string const & get<std::string>( impl::value_t const & val ) {
					return val.get_string( );
				}

				template<> bool const & get<bool>( impl::value_t const & val ) {
					return val.get_boolean( );
				}

				template<> impl::object_value const & get<impl::object_value>( impl::value_t const & val ) {
					return val.get_object( );
				}

				template<> impl::array_value const & get<impl::array_value>( impl::value_t const & val ) {
					return val.get_array( );
				}


			}	// namespace json
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw

