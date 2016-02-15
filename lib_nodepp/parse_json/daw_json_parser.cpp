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
		
			string_value create_string_value( char const * const first, char const * const last ) {
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
				std::string result { str.begin( ), str.size( ) };
				return result;
			}
 
			std::ostream& operator<<( std::ostream& os, string_value const& value ) {
				os << to_string( value );
				return os;
			}

			void value_t::u_value_t::clear( ) {
				memset( this, 0, sizeof( u_value_t ) );
			}

			value_t::value_t( ) : m_value_type( value_types::null ) {
				m_value.clear( );
			}

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

			value_t::value_t( std::nullptr_t ) : m_value_type( value_types::null ) {
				m_value.clear( );
			}

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

			value_t::value_t( value_t && other ) :
				m_value( std::move( other.m_value ) ),
				m_value_type( std::move( other.m_value_type ) ) {
				clear( other.m_value.string );
				other.m_value_type = value_types::null;
			}

			value_t & value_t::operator=(value_t && rhs) {
				if( this != &rhs ) {
					m_value = std::move( rhs.m_value );
					m_value_type = std::move( rhs.m_value_type );
					clear( rhs.m_value.string );
					rhs.m_value_type = value_types::null;
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
				if( m_value_type == value_types::array ) {
					delete m_value.array_v;
				} else if( m_value_type == value_types::object ) {
					delete m_value.object;
				}
				m_value_type = value_types::null;
				m_value.clear( );
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

		}	// namespace impl

		json_obj parse_json( boost::string_ref const json_text ) {
			auto range = range::make_range( json_text.begin( ), json_text.end( ) );
			return parse_json( range );
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


