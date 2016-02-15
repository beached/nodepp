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
#include <boost/variant.hpp>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <string>

#include "daw_common_mixins.h"
#include "daw_range.h"

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
			size_t hash_sequence( char const * first, char const * const last );
			class value_t;
			struct null_value final { };
			
			using string_value = daw::range::Range<char const *>;

			bool operator==( string_value const & first, string_value const & second );
			bool operator==( string_value const & first, boost::string_ref const & second );

			string_value create_string_value( char const * const first, char const * const last );
			string_value create_string_value( boost::string_ref const & str );


			void clear( string_value & str );
			std::string to_string( string_value const & str );
			std::ostream& operator<<( std::ostream& os, string_value const & value );
			
			boost::string_ref to_string_ref( string_value const & str );

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
						string_value string;
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

		namespace impl {
			using namespace daw::range;

			template<typename Iterator>
			bool contains( Iterator first, Iterator last, typename std::iterator_traits<Iterator>::value_type const & key ) {
				return std::find( first, last, key ) != last;
			}

			template<typename Iterator>
			bool is_ws( Iterator it ) {
				static const std::array<typename std::iterator_traits<Iterator>::value_type, 4> ws_chars = { {0x20, 0x09, 0x0A, 0x0D} };
				return contains( ws_chars.cbegin( ), ws_chars.cend( ), *it );
			}

			template<typename T>
			T lower_case( T val ) {
				return val | ' ';
			}

			template<typename Iterator>
			bool is_equal( Iterator it, typename std::iterator_traits<Iterator>::value_type val ) {
				return *it == val;
			}

			template<typename Iterator>
			bool is_equal_nc( Iterator it, typename std::iterator_traits<Iterator>::value_type val ) {
				return lower_case( *it ) == lower_case( val );
			}

			template<typename Iterator>
			void skip_ws( Range<Iterator> & range ) {
				while( range.begin( ) != range.end( ) && is_ws( range.begin( ) ) ) {
					range.move_next( );
				}
			}

			template<typename Iterator>
			bool move_range_forward_if_equal( Range<Iterator>& range, boost::string_ref value ) {
				auto const value_size = static_cast<typename std::iterator_traits<Iterator>::difference_type>(value.size( ));
				bool result = std::distance( range.begin( ), range.end( ) ) >= value_size;
				result = result && std::equal( range.begin( ), range.begin( ) + value_size, std::begin( value ) );
				if( result ) {
					safe_advance( range, value_size );
				}
				return result;
			}

			template<typename Iterator>
			value_t parse_string( Range<Iterator>& range ) {
				if( !is_equal( range.begin( ), '"' ) ) {
					throw JsonParserException( "Not a valid JSON string" );
				}
				range.move_next( );
				auto first = range.begin( );
				size_t slash_count = 0;
				while( !at_end( range ) ) {
					auto const & cur_val = range.front( );
					if( '"' == cur_val && slash_count % 2 == 0 ) {
						break;
					}
					slash_count = '\\' == cur_val ? slash_count + 1 : 0;
					range.move_next( );
				}
				if( at_end( range ) ) {
					throw JsonParserException( "Not a valid JSON string" );
				}
				auto result = value_t( create_string_value( first, range.begin( ) ) );
				range.move_next( );
				return result;
			}

			template<typename Iterator>
			value_t parse_bool( Range<Iterator>& range ) {
				if( move_range_forward_if_equal( range, "true" ) ) {
					return value_t( true );
				} else if( move_range_forward_if_equal( range, "false" ) ) {
					return value_t( false );
				}
				throw JsonParserException( "Not a valid JSON bool" );
			}

			template<typename Iterator>
			value_t parse_null( Range<Iterator> & range ) {
				if( !move_range_forward_if_equal( range, "null" ) ) {
					throw JsonParserException( "Not a valid JSON null" );
				}
				return value_t( nullptr );
			}

			template<typename Iterator>
			bool is_digit( Iterator it ) {
				auto const & test = *it;
				return '0' <= test && test <= '9';
			}

			template<typename Iterator>
			value_t parse_number( Range<Iterator> & range ) {
				auto const first = range.begin( );
				move_range_forward_if_equal( range, "-" );

				while( !at_end( range ) && is_digit( range.begin( ) ) ) {
					range.move_next( );
				}
				bool const is_float = !at_end( range ) && '.' == range.front( );
				if( is_float ) {
					range.move_next( );
					while( !at_end( range ) && is_digit( range.begin( ) ) ) { range.move_next( ); };
					if( is_equal_nc( range.begin( ), 'e' ) ) {
						range.move_next( );
						if( '-' == range.front( ) ) {
							range.move_next( );
						}
						while( !at_end( range ) && is_digit( range.begin( ) ) ) { range.move_next( ); };
					}
				}
				if( first == range.begin( ) ) {
					throw JsonParserException( "Not a valid JSON number" );
				}

				if( is_float ) {
					try {
						assert( first <= range.begin( ) );
						auto result = value_t( boost::lexical_cast<double>(first, static_cast<size_t>(std::distance( first, range.begin( ) ))) );
						return result;
					} catch( boost::bad_lexical_cast const & ) {
						throw JsonParserException( "Not a valid JSON number" );
					}
				}
				try {
					assert( first <= range.begin( ) );
					auto result = value_t( boost::lexical_cast<int64_t>(first, static_cast<size_t>(std::distance( first, range.begin( ) ))) );
					return result;
				} catch( boost::bad_lexical_cast const & ) {
					throw JsonParserException( "Not a valid JSON number" );
				}
			}

			template<typename Iterator>
			value_t parse_value( Range<Iterator>& range );

			template<typename Iterator>
			object_value_item parse_object_item( Range<Iterator> & range ) {
				auto label = parse_string( range );
				auto const & lbl = label.get_string_value( );
				skip_ws( range );
				if( !is_equal( range.begin( ), ':' ) ) {
					throw JsonParserException( "Not a valid JSON object item" );
				}
				skip_ws( range.move_next( ) );
				auto value = parse_value( range );
				return std::make_pair( lbl, value );
			}

			template<typename Iterator>
			value_t parse_object( Range<Iterator> & range ) {
				if( !is_equal( range.begin( ), '{' ) ) {
					throw JsonParserException( "Not a valid JSON object" );
				}
				range.move_next( );
				object_value result;
				do {
					skip_ws( range );
					result.push_back( parse_object_item( range ) );
					skip_ws( range );
					if( !is_equal( range.begin( ), ',' ) ) {
						break;
					}
					range.move_next( );
				} while( !at_end( range ) );
				if( !is_equal( range.begin( ), '}' ) ) {
					throw JsonParserException( "Not a valid JSON object" );
				}
				range.move_next( );
				result.shrink_to_fit( );
				return value_t( std::move( result ) );
			}

			template<typename Iterator>
			value_t parse_array( Range<Iterator>& range ) {
				if( !is_equal( range.begin( ), '[' ) ) {
					throw JsonParserException( "Not a valid JSON array" );
				}
				range.move_next( );
				array_value results;
				do {
					skip_ws( range );
					results.push_back( parse_value( range ) );
					skip_ws( range );
					if( !is_equal( range.begin( ), ',' ) ) {
						break;
					}
					range.move_next( );
				} while( !range.at_end( ) );
				if( !is_equal( range.begin( ), ']' ) ) {
					throw JsonParserException( "Not a valid JSON array" );
				}
				range.move_next( );
				results.shrink_to_fit( );
				return value_t( std::move( results ) );
			}

			template<typename Iterator>
			value_t parse_value( Range<Iterator>& range ) {
				value_t result;
				skip_ws( range );
				switch( *range.begin( ) ) {
				case '{':
					result = parse_object( range );
					break;
				case '[':
					result = parse_array( range );
					break;
				case '"':
					result = parse_string( range );
					break;
				case 't':
				case 'f':
					result = parse_bool( range );
					break;
				case 'n':
					result = parse_null( range );
					break;
				default:
					result = parse_number( range );
				}
				skip_ws( range );
				return result;
			}
		}

		template<typename Iterator>
		json_obj parse_json( daw::range::Range<Iterator> json_text ) {
			try {
				return impl::parse_value( json_text );
			} catch( JsonParserException const & ) {
				return impl::value_t( nullptr );
			}
		}

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

namespace std {
	template<>
	struct hash<daw::json::impl::string_value> {
		size_t operator()( daw::json::impl::string_value const & value ) const {
			return daw::json::impl::hash_sequence( value.begin( ), value.end( ) );
		}
	};
}
