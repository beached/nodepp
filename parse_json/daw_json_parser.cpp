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

#include "daw_range.h"
#include "daw_json_interface.h"
#include "daw_json_parser.h"

namespace daw {
	namespace json {
		namespace impl {
			using namespace daw::range;

			value_t::value_t( ) : m_value_type( value_types::null ) {
				m_value.null = nullptr;
			}

			value_t::value_t( int64_t const & value ) : m_value_type( value_types::integral ) {
				m_value.integral = value;
			}

			value_t::value_t( double const & value ) : m_value_type( value_types::real ) {
				m_value.real = value;
			}

			value_t::value_t( std::string value ) : m_value_type( value_types::string ) {
				m_value.string = new std::string( std::move( value ) );
			}

			value_t::value_t( boost::string_ref value ): m_value_type( value_types::string ) {
				m_value.string = new std::string( value.to_string( ) );
			}

			value_t::value_t( bool value ) : m_value_type( value_types::boolean ) {
				m_value.boolean = std::move( value );
			}

			value_t::value_t( std::nullptr_t ) : m_value_type( value_types::null ) {
				m_value.null = nullptr;
			}

			value_t::value_t( array_value value ) : m_value_type( value_types::array ) {
				m_value.array = new array_value( std::move( value ) );
			}

			value_t::value_t( object_value value ) : m_value_type( value_types::object ) {
				m_value.object = new object_value( std::move( value ) );
			}

			value_t::value_t( value_t const & other ): m_value_type( other.m_value_type ) {
				switch( m_value_type ) {
				case value_types::string:
					m_value.string = new std::string( *other.m_value.string );
					break;
				case value_types::array:
					m_value.array = new array_value( *other.m_value.array );
					break;
				case value_types::object:
					m_value.object = new object_value( *other.m_value.object );
					break;
				default:
					m_value = other.m_value;
				}
			}

			value_t & value_t::operator=(value_t const & rhs) {
				if( this != &rhs ) {
					m_value_type = rhs.m_value_type;
					switch( m_value_type ) {
					case value_types::string:
						m_value.string = new std::string( *rhs.m_value.string );
						break;
					case value_types::array:
						m_value.array = new array_value( *rhs.m_value.array );
						break;
					case value_types::object:
						m_value.object = new object_value( *rhs.m_value.object );
						break;
					default:
						m_value = rhs.m_value;
					}
				}

				return *this;
			}

			value_t::value_t( value_t && other ) :
				m_value( std::move( other.m_value ) ),
				m_value_type( std::move( other.m_value_type ) ) {
				other.m_value.string = nullptr;
				other.m_value_type = value_types::null;
			}

			value_t & value_t::operator=(value_t && rhs) {
				if( this != &rhs ) {
					m_value = std::move( rhs.m_value );
					m_value_type = std::move( rhs.m_value_type );
					rhs.m_value.string = nullptr;
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

			std::string const & value_t::get_string( ) const {
				assert( m_value_type == value_types::string );
				assert( m_value.string != nullptr );
				return *m_value.string;
			}

			std::string & value_t::get_string( ) {
				assert( m_value_type == value_types::string );
				assert( m_value.string != nullptr );
				return *m_value.string;
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
				assert( m_value.object != nullptr );
				return *m_value.object;
			}

			object_value & value_t::get_object( ) {
				assert( m_value_type == value_types::object );
				assert( m_value.object != nullptr );
				return *m_value.object;
			}

			array_value const & value_t::get_array( ) const {
				assert( m_value_type == value_types::array );
				assert( m_value.array != nullptr );
				return *m_value.array;
			}

			array_value & value_t::get_array( ) {
				assert( m_value_type == value_types::array );
				assert( m_value.array != nullptr );
				return *m_value.array;
			}

			void value_t::cleanup( ) {
				switch( m_value_type ) {
				case value_types::string:
					if( m_value.string != nullptr ) {
						delete m_value.string;
						m_value.string = nullptr;
						m_value_type = value_types::null;
					}
					break;
				case value_types::object:
					if( m_value.object != nullptr ) {
						delete m_value.object;
						m_value.object = nullptr;
						m_value_type = value_types::null;
					}

					break;
				case value_types::array:
					if( m_value.array != nullptr ) {
						delete m_value.array;
						m_value.array = nullptr;
						m_value_type = value_types::null;
					}
					break;
				}
			}

			value_t::value_types value_t::type( ) const {
				return m_value_type;
			}

			std::ostream& operator<<(std::ostream& os, value_t const & value) {
				switch( value.type( ) ) {
				case value_t::value_types::array: {
					os << "[ ";
					const auto & arry = value.get_array( ).items;
					if( !arry.empty( ) ) {
						os << arry[0];
						for( size_t n = 1; n < arry.size( ); ++n ) {
							os << ", " << arry[n];
						}
					}
					os << " ]";
				}
												  break;
				case value_t::value_types::boolean:
					os << (value.get_boolean( ) ? "True" : "False");
					break;
				case value_t::value_types::integral:
					os << value.get_integral( );
					break;
				case value_t::value_types::null:
					os << "null";
					break;
				case value_t::value_types::object: {
					os << "{ ";
					const auto & items = value.get_object( ).members;
					if( !items.empty( ) ) {
						os << '"' << items[0].first << "\" : " << items[0].second;
						for( size_t n = 1; n < items.size( ); ++n ) {
							os << ", \"" << items[n].first << "\" : " << items[n].second;
						}
					}
					os << " }";
				}
												   break;
				case value_t::value_types::real:
					os << value.get_real( );
					break;
				case value_t::value_types::string:
					os << '"' << value.get_string( ) << '"';
					break;
				default:
					throw std::runtime_error( "Unexpected value type" );
				}
				return os;
			}

			std::ostream& operator<<(std::ostream& os, std::shared_ptr<value_t> const & value) {
				if( value ) {
					os << *value;
				} else {
					os << "{null}";
				}
				return os;
			}

			object_value_item make_object_value_item( std::string first, value_t second ) {
				return std::make_pair<std::string, value_t>( std::move( first ), std::move( second ) );
			}

			object_value::iterator object_value::find( boost::string_ref const key ) {
				return std::find_if( members.begin( ), members.end( ), [key]( object_value_item const & item ) {
					return item.first == key;
				} );
			}

			object_value::const_iterator object_value::find( boost::string_ref const key ) const {
				return std::find_if( members.cbegin( ), members.cend( ), [key]( object_value_item const & item ) {
					return item.first == key;
				} );
			}

			// 			object_value::iterator object_value::end( ) {
			// 				return members.end( );
			// 			}
			//
			// 			object_value::const_iterator object_value::end( ) const {
			// 				return members.cend( );
			// 			}
			//
			// 			object_value::iterator object_value::begin( ) {
			// 				return members.begin( );
			// 			}
			//
			// 			object_value::const_iterator object_value::begin( ) const {
			// 				return members.cbegin( );
			// 			}

			// 			object_value::iterator object_value::insert( object_value::iterator where, object_value_item item ) {
			// 				return members.insert( where, std::move( item ) );
			// 			}

			// 			void object_value::push_back( object_value_item item ) {
			// 				members.push_back( std::move( item ) );
			// 			}
			//
			object_value::mapped_type & object_value::operator[]( boost::string_ref key ) {
				auto pos = find( key );
				if( end( ) == pos ) {
					pos = insert( pos, std::make_pair<std::string, value_t>( key.to_string( ), value_t( nullptr ) ) );
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
			//
			namespace {
				template<typename Iterator>
				bool contains( Iterator first, Iterator last, typename std::iterator_traits<Iterator>::value_type const & key ) {
					return std::find( first, last, key ) != last;
				}

				bool is_ws( char const * const it ) {
					const std::vector<char> ws_chars = { 0x20, 0x09, 0x0A, 0x0D };
					return contains( ws_chars.cbegin( ), ws_chars.cend( ), *it );
				}

				template<typename Iterator>
				bool is_equal( Iterator it, typename std::iterator_traits<Iterator>::value_type val ) {
					return *it == val;
				}

				char lower_case( char val ) {
					return val | ' ';
				}

				template<typename Iterator>
				bool is_equal_nc( Iterator it, typename std::iterator_traits<Iterator>::value_type val ) {
					return lower_case( *it ) == lower_case( val );
				}

				template<typename Iterator>
				void skip_ws( Range<Iterator> & range ) {
					while( range.first != range.last && is_ws( range.first ) ) {
						range.move_next( );
					}
				}

				template<typename Iterator>
				bool forward_if_equal( Range<Iterator>& range, boost::string_ref value ) {
					bool result = std::distance( range.first, range.last ) >= static_cast<typename std::iterator_traits<Iterator>::difference_type>(value.size( ));
					result = result && std::equal( range.first, range.first + value.size( ), std::begin( value ) );
					if( result ) {
						safe_advance( range, static_cast<typename std::iterator_traits<Iterator>::difference_type>(value.size( )) );
					}
					return result;
				}

				template<typename Iterator>
				value_opt_t parse_string( Range<Iterator>& range ) {
					auto current = range;
					if( !is_equal( current.first, '"' ) ) {
						return value_opt_t( );
					}
					current.move_next( );
					int slash_count = 0;
					while( !at_end( current ) ) {
						auto const & cur_val = current.front( );
						if( '"' == cur_val && slash_count % 2 == 0 ) {
							break;
						}
						slash_count = '\\' == cur_val ? slash_count + 1 : 0;
						current.move_next( );
					}
					if( !at_end( current ) ) {
						auto result = value_opt_t( std::string( range.first + 1, current.first ) );
						current.move_next( );
						range = current;
						return result;
					}

					return value_opt_t( );
				}

				template<typename Iterator>
				value_opt_t parse_bool( Range<Iterator>& range ) {
					if( forward_if_equal( range, "true" ) ) {
						return value_t( true );
					} else if( forward_if_equal( range, "false" ) ) {
						return value_t( false );
					}
					return value_opt_t( );
				}

				template<typename Iterator>
				value_opt_t parse_null( Range<Iterator> & range ) {
					if( forward_if_equal( range, "null" ) ) {
						return value_t( nullptr );
					}
					return value_opt_t( );
				}

				template<typename Iterator>
				bool is_digit( Iterator it ) {
					auto const & test = *it;
					return '0' <= test && test <= '9';
				}

				template<typename Iterator>
				value_opt_t parse_number( Range<Iterator> & range ) {
					auto current = range;
					if( '-' == current.front( ) ) {
						current.move_next( );
					}
					while( !at_end( current ) && is_digit( current.first ) ) { current.move_next( ); };
					bool is_float = !at_end( current ) && '.' == current.front( );
					if( is_float ) {
						current.move_next( );
						while( !at_end( current ) && is_digit( current.first ) ) { current.move_next( ); };
						if( is_equal_nc( current.first, 'e' ) ) {
							current.move_next( );
							if( '-' == current.front( ) ) {
								current.move_next( );
							}
							while( !at_end( current ) && is_digit( current.first ) ) { current.move_next( ); };
						}
					}
					if( range.first == current.first ) {
						return value_opt_t( );
					}

					if( is_float ) {
						try {
							assert( range.first <= current.first );
							auto result = value_t( boost::lexical_cast<double>(range.first, static_cast<size_t>(std::distance( range.first, current.first ))) );
							range = current;
							return result;
						} catch( boost::bad_lexical_cast const & ) { }
					} else {
						try {
							assert( range.first <= current.first );
							auto result = value_t( boost::lexical_cast<int64_t>(range.first, static_cast<size_t>(std::distance( range.first, current.first ))) );
							range = current;
							return result;
						} catch( boost::bad_lexical_cast const & ) { }
					}
					// Error return null
					return value_opt_t( );
				}

				template<typename Iterator>
				value_opt_t parse_value( Range<Iterator> & range );

				template<typename Iterator>
				boost::optional<object_value_item> parse_object_item( Range<Iterator> & range ) {
					auto current = range;
					auto label = parse_string( current );
					if( !label || !label->is_string( ) ) {
						return boost::optional<object_value_item>( );
					}
					auto lbl = label->get_string( );
					skip_ws( current );
					if( !is_equal( current.first, ':' ) ) {
						return boost::optional<object_value_item>( );
					}
					skip_ws( current.move_next( ) );
					auto value = parse_value( current );
					if( !value ) {
						return boost::optional<object_value_item>( );
					}
					range = current;

					return std::make_pair< std::string, value_t >( std::move( lbl ), std::move( *value ) );
				}

				template<typename Iterator>
				value_opt_t parse_object( Range<Iterator> & range ) {
					auto current = range;
					if( !is_equal( current.first, '{' ) ) {
						return value_opt_t( );
					}
					current.move_next( );
					object_value result;
					do {
						skip_ws( current );
						auto item = parse_object_item( current );
						if( !item ) {
							return value_opt_t( );
						}
						result.push_back( *item );
						skip_ws( current );
						if( !is_equal( current.first, ',' ) ) {
							break;
						}
						current.move_next( );
					} while( !at_end( current ) );
					if( !is_equal( current.first, '}' ) ) {
						return value_opt_t( );
					}
					current.move_next( );
					range = current;
					return value_opt_t( std::move( result ) );
				}

				template<typename Iterator>
				value_opt_t parse_array( Range<Iterator>& range ) {
					auto current = range;
					if( !is_equal( current.first, '[' ) ) {
						return value_opt_t( );
					}
					current.move_next( );
					array_value results;
					do {
						skip_ws( current );
						auto item = parse_value( current );
						if( !item ) {
							return value_opt_t( );
						}
						results.items.push_back( std::move( *item ) );
						skip_ws( current );
						if( !is_equal( current.first, ',' ) ) {
							break;
						}
						current.move_next( );
					} while( !current.at_end( ) );
					if( !is_equal( current.first, ']' ) ) {
						return value_opt_t( );
					}
					current.move_next( );
					range = current;
					return value_opt_t( results );
				}

				template<typename Iterator>
				value_opt_t parse_value( Range<Iterator>& range ) {
					auto current = range;
					skip_ws( current );
					switch( current.front( ) ) {
					case '{':
						if( auto obj = parse_object( current ) ) {
							skip_ws( current );
							range = current;
							return obj;
						}
						break;
					case '[':
						if( auto obj = parse_array( current ) ) {
							skip_ws( current );
							range = current;
							return obj;
						}
						break;
					case '"':
						if( auto obj = parse_string( current ) ) {
							skip_ws( current );
							range = current;
							return obj;
						}
						break;
					case 't':
					case 'f':
						if( auto obj = parse_bool( current ) ) {
							skip_ws( current );
							range = current;
							return obj;
						}
						break;
					case 'n':
						if( auto obj = parse_null( current ) ) {
							skip_ws( current );
							range = current;
							return obj;
						}
						break;
					default:
						if( auto obj = parse_number( current ) ) {
							skip_ws( current );
							range = current;
							return obj;
						}
					}
					return value_opt_t( );
				}
			}	// namespace anonymous
		}	// namespace impl

		json_obj parse_json( boost::string_ref const json_text ) {
			auto range = impl::make_range( json_text.begin( ), json_text.end( ) );
			auto result = impl::parse_value( range );
			if( result ) {
				return std::make_shared<impl::value_t>( std::move( *result ) );
			}
			return std::make_shared<impl::value_t>( nullptr );
		}

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
}	// namespace daw
