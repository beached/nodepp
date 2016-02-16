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

#include <boost/lexical_cast.hpp>
#include <boost/utility/string_ref.hpp>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

#include "daw_range.h"
#include "daw_json_interface.h"
#include "daw_json_parser.h"

namespace daw {
	namespace json {
		JsonParserException::JsonParserException(std::string msg): message( std::move( msg ) ) { }
		
		namespace {
			template<typename T>
			T* copy_value( T* ptr ) {
				return new T( *ptr );
			}
		}	// namespace anonymous

		namespace impl {
			
			using namespace daw::range;
			using CharIterator = char const *;

			size_t hash_sequence( char const * first, char const * const last ) {
				// FNV-1a hash function for bytes in [fist, last], see http://www.isthe.com/chongo/tech/comp/fnv/index.html
			#if defined(_WIN64) || defined(__amd64__)
				static_assert(sizeof( size_t ) == 8, "This code is for 64-bit size_t.");
				size_t const fnv_offset_basis = 14695981039346656037ULL;
				size_t const fnv_prime = 1099511628211ULL;

			#elif defined(_WIN32) || defined(__i386__)
				static_assert(sizeof( size_t ) == 4, "This code is for 32-bit size_t.");
				size_t const fnv_offset_basis = 2166136261U;
				size_t const fnv_prime = 16777619U;
			#else
				#error "Unknown platform for hash"
			#endif 

				auto result = fnv_offset_basis;
				for( ; first <= last; ++first ) {
					result ^= static_cast<size_t>(*first);
					result *= fnv_prime;
				}
				return result;
			}			
		
			string_value create_string_value( CharIterator first, CharIterator last ) {
				return { first, last };
			}

			string_value create_string_value( boost::string_ref const& str ) {
				return { str.begin( ), str.end( ) };
			}

			bool operator==( string_value const & first, string_value const & second ) {
				return std::equal( first.begin( ), first.end( ), second.begin( ) );
			}
 
			bool operator==( string_value const & first, boost::string_ref const & second ) {
				return std::equal( first.begin( ), first.end( ), second.begin( ) );
			}
 
			void clear( string_value & str ) {
				str = { nullptr, nullptr };
			}

			std::string to_string( string_value const & str ) {
				return std::string{ str.begin( ), str.size( ) };
			}
 
			std::ostream& operator<<( std::ostream& os, string_value const& value ) {
				for( auto const & c : value ) {
					os << c;
				}
				return os;
			}

			void value_t::u_value_t::clear( ) {
				memset( this, 0, sizeof( u_value_t ) );
			}

			value_t::value_t( ) : m_value_type( value_types::null ) { }

			value_t::value_t( int64_t const & value ) : m_value_type( value_types::integral ) {				
				m_value.integral = value;
			}

			value_t::value_t( double const & value ) : m_value_type( value_types::real ) {
				m_value.real = value;
			}

			value_t::value_t( std::string const & value ) : m_value_type( value_types::string ) {
				m_value.string = create_string_value( value );
			}

			value_t::value_t( boost::string_ref value ): m_value_type( value_types::string ) {
				m_value.string = { value.begin( ), value.end( ) };
			}

			value_t::value_t( string_value value ): m_value_type( value_types::string ) {
				m_value.string = value;
			}

			value_t::value_t( bool value ) : m_value_type( value_types::boolean ) {
				m_value.boolean = std::move( value );
			}

			value_t::value_t( std::nullptr_t ) : m_value_type( value_types::null ) { }

			value_t::value_t( array_value value ) : m_value_type( value_types::array ) {
				m_value.array_v = new array_value( std::move( value ) );
			}

			value_t::value_t( object_value value ) : m_value_type( value_types::object ) {
				m_value.object = new object_value( std::move( value ) );
			}

			value_t::value_t( value_t const & other ): m_value_type( other.m_value_type ) {
				switch( m_value_type ) {
				case value_types::string:
					m_value.string = other.m_value.string;
					break;
				case value_types::array:
					m_value.array_v = copy_value( other.m_value.array_v );
					break;
				case value_types::object:
					m_value.object = copy_value( other.m_value.object );
					break;
				case value_types::integral:
					m_value.integral = other.m_value.integral;
					break;
				case value_types::real:
					m_value.real = other.m_value.real;
					break;
				case value_types::boolean:
					m_value.boolean = other.m_value.boolean;
					break;
				case value_types::null:
					break;
				default:
					throw std::runtime_error( "Unknown value_t type" );
				}
			}

			value_t & value_t::operator=(value_t const & rhs) {
				if( this != &rhs ) {
					m_value_type = rhs.m_value_type;
					switch( m_value_type ) {
					case value_types::string:
						m_value.string = rhs.m_value.string;
						break;
					case value_types::array:
						m_value.array_v = copy_value( rhs.m_value.array_v );
						break;
					case value_types::object:
						m_value.object = copy_value( rhs.m_value.object );
						break;
					case value_types::integral:
						m_value.integral = rhs.m_value.integral;
						break;
					case value_types::real:
						m_value.real = rhs.m_value.real;
						break;
					case value_types::boolean:
						m_value.boolean = rhs.m_value.boolean;
						break;
					case value_types::null:
						break;
					default:
						throw std::runtime_error( "Unknown value_t type" );
					}
				}

				return *this;
			}

			value_t::value_t( value_t && other ) : m_value_type( other.m_value_type ) {
				switch( m_value_type ) {
				case value_types::array:
					m_value.array_v = other.m_value.array_v;
					other.m_value_type = value_types::null;
					break;
				case value_types::object: 
					m_value.object = other.m_value.object;
					other.m_value_type = value_types::null;
					break;
				case value_types::string:
					m_value.string = other.m_value.string;
					break;
				case value_types::integral:
					m_value.integral = other.m_value.integral;
					break;
				case value_types::real:
					m_value.real = other.m_value.real;
					break;
				case value_types::boolean:
					m_value.boolean = other.m_value.boolean;
					break;
				case value_types::null: break;
				default:
					throw std::runtime_error( "Unexpected value_t type" );
				}
			}

			value_t & value_t::operator=(value_t && rhs) {
				if( this != &rhs ) {
					m_value_type = rhs.m_value_type;
					switch( m_value_type ) {
					case value_types::array:
						m_value.array_v = rhs.m_value.array_v;
						rhs.m_value_type = value_types::null;
						break;
					case value_types::object:
						m_value.object = rhs.m_value.object;
						rhs.m_value_type = value_types::null;
						break;
					case value_types::string:
						m_value.string = rhs.m_value.string;
						break;
					case value_types::integral:
						m_value.integral = rhs.m_value.integral;
						break;
					case value_types::real:
						m_value.real = rhs.m_value.real;
						break;
					case value_types::boolean:
						m_value.boolean = rhs.m_value.boolean;
						break;
					case value_types::null: break;
					default:
						throw std::runtime_error( "Unexpected value_t type" );
					}
				}
				return *this;
			}

			value_t::~value_t( ) {
				cleanup( );
			}

			bool const & value_t::get_boolean( ) const {
				assert( m_value_type == value_types::boolean );
				return m_value.boolean;
			}

			bool & value_t::get_boolean( ) {
				assert( m_value_type == value_types::boolean );
				return m_value.boolean;
			}

			int64_t const & value_t::get_integral( ) const {
				assert( m_value_type == value_types::integral );
				return m_value.integral;
			}

			int64_t & value_t::get_integral( ) {
				assert( m_value_type == value_types::integral );
				return m_value.integral;
			}

			double const & value_t::get_real( ) const {
				assert( m_value_type == value_types::real );
				return m_value.real;
			}

			double & value_t::get_real( ) {
				assert( m_value_type == value_types::real );
				return m_value.real;
			}

			std::string value_t::get_string( ) const {
				assert( m_value_type == value_types::string );
				assert( m_value.string.begin( ) != nullptr );
				return to_string( m_value.string );
			}

			string_value value_t::get_string_value() const {
				assert( m_value_type == value_types::string );
				assert( m_value.string.begin( ) != nullptr );
				return m_value.string;
			}

			bool value_t::is_integral( ) const {
				return m_value_type == value_types::integral;
			}

			bool value_t::is_real( ) const {
				return m_value_type == value_types::real;
			}

			bool value_t::is_string( ) const {
				return m_value_type == value_types::string;
			}

			bool value_t::is_boolean( ) const {
				return m_value_type == value_types::boolean;
			}

			bool value_t::is_null( ) const {
				return m_value_type == value_types::null;
			}

			bool value_t::is_array( ) const {
				return m_value_type == value_types::array;
			}

			bool value_t::is_object( ) const {
				return m_value_type == value_types::object;
			}

			object_value const & value_t::get_object( ) const {
				assert( m_value_type == value_types::object );
				assert( m_value.object );
				return *m_value.object;
			}

			object_value & value_t::get_object( ) {
				assert( m_value_type == value_types::object );
				assert( m_value.object );
				return *m_value.object;
			}

			array_value const & value_t::get_array( ) const {
				assert( m_value_type == value_types::array );
				assert( m_value.array_v );
				return *m_value.array_v;
			}

			array_value & value_t::get_array( ) {
				assert( m_value_type == value_types::array );
				assert( m_value.array_v );
				return *m_value.array_v;
			}

			void value_t::cleanup( ) {
				if( m_value_type == value_types::array && m_value.array_v != nullptr ) {
					delete m_value.array_v;
					m_value.array_v = nullptr;
				} else if( m_value_type == value_types::object && m_value.object != nullptr ) {
					delete m_value.object;
					m_value.object = nullptr;
				}
				m_value_type = value_types::null;
			}

			value_t::value_types value_t::type( ) const {
				return m_value_type;
			}

			std::string to_string( value_t const & value) {
				std::stringstream ss;
				switch( value.type( ) ) {
				case value_t::value_types::array: {
					ss <<"[ ";
					const auto & arry = value.get_array( );
					if( !arry.empty( ) ) {
						ss << arry[0];
						for( size_t n = 1; n <arry.size( ); ++n ) {
							ss << ", " << arry[n];
						}
					}
					ss <<" ]";
				}
												  break;
				case value_t::value_types::boolean:
					ss <<(value.get_boolean( ) ? "True" : "False");
					break;
				case value_t::value_types::integral:
					ss <<value.get_integral( );
					break;
				case value_t::value_types::null:
					ss <<"null";
					break;
				case value_t::value_types::object: {
					ss <<"{ ";
					const auto & items = value.get_object( ).members_v;
					if( !items.empty( ) ) {
						ss <<'"' <<items[0].first <<"\" : " <<items[0].second;
						for( size_t n = 1; n <items.size( ); ++n ) {
							ss <<", \"" <<items[n].first <<"\" : " <<items[n].second;
						}
					}
					ss <<" }";
				}
												   break;
				case value_t::value_types::real:
					ss <<value.get_real( );
					break;
				case value_t::value_types::string:
					ss <<'"' <<value.get_string( ) <<'"';
					break;
				default:
					throw std::runtime_error( "Unexpected value type" );
				}
				return ss.str( );
			}

			std::string to_string( std::shared_ptr<value_t> const & value ) {
				if( value ) {
					return to_string( *value );
				}
				return "{ null }";
			}

			std::ostream& operator<<( std::ostream& os, value_t const & value ) {
				os <<to_string( value );
				return os;
			}

			std::ostream& operator<<(std::ostream& os, std::shared_ptr<value_t> const & value) {
				os <<to_string( value );
				return os;
			}

			boost::string_ref to_string_ref(string_value const& str) {
				return { str.begin( ), str.size( ) };
			}

			object_value_item make_object_value_item( string_value first, value_t second ) {
				return std::make_pair<string_value, value_t>( std::move( first ), std::move( second ) );
			}

			object_value::iterator object_value::find( boost::string_ref const key ) {
				return std::find_if( members_v.begin( ), members_v.end( ), [key]( object_value_item const & item ) {
					return item.first == key;
				} );
			}

			object_value::const_iterator object_value::find( boost::string_ref const key ) const {
				return std::find_if( members_v.begin( ), members_v.end( ), [key]( object_value_item const & item ) {
					return item.first == key;
				} );
			}
			
			object_value::mapped_type & object_value::operator[]( boost::string_ref key ) {
				auto pos = find( key );
				if( end( ) == pos ) {
					pos = insert( pos, std::make_pair<string_value, value_t>( create_string_value( key ), value_t( nullptr ) ) );
				}
				return pos->second;
			}

			object_value::mapped_type const & object_value::operator[]( boost::string_ref key ) const {
				auto pos = find( key );
				if( end( ) == pos ) {
					throw std::out_of_range( "Attempt to access an undefined value in a const object" );
				}
				return pos->second;
			}

			template<typename Iterator>
			bool contains( Iterator first, Iterator last, typename std::iterator_traits<Iterator>::value_type const & key ) {
				return std::find( first, last, key ) != last;
			}

			bool is_ws( char val ) {
				auto result = 0x09 - val == 0;
				result |= 0x0A - val == 0;
				result |= 0x0D - val == 0;
				result |= 0x20 - val == 0;
				return result;
			}

			char ascii_lower_case( char val ) {
				return val | ' ';
			}

			bool is_equal( CharIterator it, typename std::iterator_traits<CharIterator>::value_type val ) {
				return *it == val;
			}

			bool is_equal_nc( CharIterator it, typename std::iterator_traits<CharIterator>::value_type val ) {
				return ascii_lower_case( *it ) == ascii_lower_case( val );
			}

// 			void skip_ws( Range<CharIterator> & range ) {
// 				auto it_begin = range.begin( );
// 				auto const it_end = range.end( );
// 				while( it_begin != it_end && is_ws( *it_begin ) ) { 
// 					++it_begin; 
// 				}
// 				range.set_begin( it_begin );
// 			}

			void skip_ws( Range<CharIterator> & range ) {
				auto it_begin = range.begin( );
				auto const it_end = range.end( );
				if( std::distance( it_begin, it_end ) > 3 ) {
					auto inc = static_cast<int>(is_ws( *it_begin ));
					std::advance( it_begin, inc );
					inc = inc*(static_cast<int>(is_ws( *it_begin )));
					std::advance( it_begin, inc );
					inc = inc*(static_cast<int>(is_ws( *it_begin )));
				}
				while( it_begin != it_end && is_ws( *it_begin ) ) {
					++it_begin;
				}
				range.set_begin( it_begin );
			}

			bool move_range_forward_if_equal( Range<CharIterator>& range, boost::string_ref const value ) {
				auto const value_size = static_cast<typename std::iterator_traits<CharIterator>::difference_type>( std::distance( value.begin( ), value.end( ) ) );
				auto result = std::equal( value.begin( ), value.end( ), range.begin( ) );
				if( result ) {
					safe_advance( range, value_size );
				}
				return result;
			}

			value_t parse_string( Range<CharIterator>& range ) {
				auto it_begin = range.begin( );
				if( !is_equal( it_begin, '"' ) ) {
					throw JsonParserException( "Not a valid JSON string" );
				}
				++it_begin;
				auto const it_first = it_begin;
				auto it_end = range.end( );
				size_t slash_count = 0;
				while( it_begin != it_end ) {
					auto const & cur_val = *it_begin;
					if( '"' == cur_val && slash_count % 2 == 0 ) {
						break;
					}
					slash_count = '\\' == cur_val ? slash_count + 1 : 0;
					++it_begin;
				}
				if( !(it_begin != it_end) ) {
					throw JsonParserException( "Not a valid JSON string" );
				}
				auto result = value_t( create_string_value( it_first, it_begin ) );
				++it_begin;
				range.set_begin( it_begin );
				return result;
			}

			value_t parse_bool( Range<CharIterator>& range ) {
				if( move_range_forward_if_equal( range, "true" ) ) {
					return value_t( true );
				} else if( move_range_forward_if_equal( range, "false" ) ) {
					return value_t( false );
				}
				throw JsonParserException( "Not a valid JSON bool" );
			}

			value_t parse_null( Range<CharIterator> & range ) {
				if( !move_range_forward_if_equal( range, "null" ) ) {
					throw JsonParserException( "Not a valid JSON null" );
				}
				return value_t( nullptr );
			}

			bool is_digit( CharIterator it ) {
				auto const & test = *it;
				return '0' <= test && test <= '9';
			}

			value_t parse_number( Range<CharIterator> & range ) {
				auto const first = range.begin( );
				move_range_forward_if_equal( range, "-" );

				while( !at_end( range ) && is_digit( range.begin( ) ) ) {
					range.move_next( );
				}
				bool const is_float = !at_end( range ) && '.' == *range;
				if( is_float ) {
					range.move_next( );
					while( !at_end( range ) && is_digit( range.begin( ) ) ) { range.move_next( ); };
					if( is_equal_nc( range.begin( ), 'e' ) ) {
						range.move_next( );
						if( '-' == *range ) {
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
						auto result = value_t( boost::lexical_cast<double>(first, static_cast<size_t>(std::distance( first, range.begin( ) ))) );
						return result;
					} catch( boost::bad_lexical_cast const & ) {
						throw JsonParserException( "Not a valid JSON number" );
					}
				}
				try {
					auto result = value_t( boost::lexical_cast<int64_t>(first, static_cast<size_t>(std::distance( first, range.begin( ) ))) );
					return result;
				} catch( boost::bad_lexical_cast const & ) {
					throw JsonParserException( "Not a valid JSON number" );
				}
			}

			value_t parse_value( Range<CharIterator>& range );

			object_value_item parse_object_item( Range<CharIterator> & range ) {
				auto label = parse_string( range ).get_string_value();
				skip_ws( range );
				if( !is_equal( range.begin( ), ':' ) ) {
					throw JsonParserException( "Not a valid JSON object item" );
				}
				skip_ws( range.move_next( ) );
				auto value = parse_value( range );
				return std::make_pair( std::move( label ), std::move( value ) );
			}

			value_t parse_object( Range<CharIterator> & range ) {
				if( !is_equal( range.begin( ), '{' ) ) {
					throw JsonParserException( "Not a valid JSON object" );
				}
				range.move_next( );
				object_value result;
				do {
					skip_ws( range );
					if( is_equal( range.begin(), '"' ) ) {
						result.push_back( parse_object_item( range ) );
						skip_ws( range );
					}
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

			value_t parse_array( Range<CharIterator>& range ) {
				if( !is_equal( range.begin( ), '[' ) ) {
					throw JsonParserException( "Not a valid JSON array" );
				}
				range.move_next( );
				array_value results;
				do {
					skip_ws( range );
					if( !is_equal( range.begin( ), ']' ) ) {
						results.push_back( parse_value( range ) );
						skip_ws( range );
					}
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


			value_t parse_value( Range<CharIterator>& range ) {
				value_t result;
				skip_ws( range );
				switch( *range ) {
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

		}	// namespace impl

		json_obj parse_json(char const* Begin, char const* End) {
			try {
				auto range = range::make_range( Begin, End );
				return impl::parse_value( range );
			} catch( JsonParserException const & ) {
				return impl::value_t( nullptr );
			}
		}

		json_obj parse_json( boost::string_ref const json_text ) {
			return parse_json( json_text.begin( ), json_text.end( ) );
		}

		template<> int64_t get<int64_t>( impl::value_t const & val ) {
			return val.get_integral( );
		}

		template<> double get<double>( impl::value_t const & val ) {
			return val.get_real( );
		}

		template<> std::string get<std::string>( impl::value_t const & val ) {
			return val.get_string( );
		}

		template<> bool get<bool>( impl::value_t const & val ) {
			return val.get_boolean( );
		}

		template<> impl::object_value get<impl::object_value>( impl::value_t const & val ) {
			return val.get_object( );
		}

		template<> impl::array_value get<impl::array_value>( impl::value_t const & val ) {
			return val.get_array( );
		}

	}	// namespace json
}	// namespace daw


