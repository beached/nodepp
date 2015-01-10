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

#include "base_json_parser.h"

namespace daw {
	namespace nodepp {
		namespace base {
			namespace json {
				namespace impl {
					object_value::iterator object_value::find( boost::string_ref const key ) {
						return std::find_if( members.begin( ), members.end( ), [key]( object_value_item const & item ) {							
							return item.first == key;
						} );
					}

					object_value::iterator object_value::end( ) {
						return members.end( );
					}

					object_value::iterator object_value::begin( ) {
						return members.begin( );
					}

					namespace {
						template<typename Iterator>
						struct Range {
							Iterator first;
							Iterator last;

							Range& move_next( ) {
								assert( first != last );
								++first;								
								return *this;
							}

							bool at_end( ) const {
								return first == last;
							}

						};	// struct Range

						template<typename Iterator>
						Range<Iterator> make_range( Iterator first, Iterator last ) {
							return Range < Iterator > { first, last };
						}

						template<typename Iterator>
						void safe_advance( Range<Iterator> & range, typename std::iterator_traits<Iterator>::difference_type count ) {
							assert( 0 <= count );
							if( std::distance( range.first, range.last ) >= count ) {
								range.first += count;
							} else {
								range.first = range.last;
							}
						}

						template<typename Iterator>
						bool contains( Iterator first, Iterator last, typename std::iterator_traits<Iterator>::value_type const & key ) {
							return std::find( first, last, key ) != last;
						}

						template<typename Iterator>
						bool contains( Range<Iterator> const & range, typename std::iterator_traits<Iterator>::value_type const & key ) {
							return std::find( range.first, range.last, key ) != range.last;
						}

						template<typename Iterator>
						bool at_end( Range<Iterator> const & range ) {
							return range.first == range.last;
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
							bool result = std::distance( range.first, range.last ) >= static_cast<typename std::iterator_traits<Iterator>::difference_type>( value.size( ) );
							result = result && std::equal( range.first, range.first + value.size( ), std::begin( value ) );
							if( result ) {
								safe_advance( range, value.size( ) );
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
							while( !at_end( current ) ) {
								if( is_equal( current.first, '"' ) && !is_equal( current.first - 1, '\\' ) ) {
									break;
								}
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
								return value_t( null_value( ) );
							}
							return value_opt_t( );
						}

						template<typename Iterator>
						bool is_digit( Iterator it ) {
							return 0 <= *it && *it <= 9;
						}

						template<typename Iterator>
						value_opt_t parse_number( Range<Iterator> & range ) {
							auto current = range;
							while( !at_end( current ) && is_digit( current.first ) ) { current.move_next( ); };
							if( range.first == current.first ) {
								return value_opt_t( );
							}
							if( !is_equal( current.first, '.' ) ) {
								if( at_end( current ) || is_ws( current.first ) ) {
									try {
										auto result = value_t( boost::lexical_cast<int64_t>(range.first, std::distance( range.first, current.first ) - 1) );
										range = current;
										return result;
									} catch( boost::bad_lexical_cast const & ) { }
								}
								// Error
								return value_opt_t( );
							}

							current.move_next( );
							if( !is_digit( current.first ) ) {
								// Error
								return value_opt_t( );
							}
							while( !at_end( current ) && is_digit( current.first ) ) { current.move_next( ); }
							if( is_ws( current.first ) ) {
								// Double
								try {
									auto result = value_t( boost::lexical_cast<double>(range.first, std::distance( range.first, current.first ) - 1) );
									range = current;
									return result;
								} catch( boost::bad_lexical_cast const & ) {
									return value_opt_t( );
								}
							}
							if( is_equal_nc( current.first, 'e' ) ) {
								current.move_next( );
								while( !at_end( current ) && is_digit( current.first ) ) { current.move_next( ); }
								if( is_ws( current.first ) ) {
									try {
										auto result = value_t( boost::lexical_cast<double>(range.first, std::distance( range.first, current.first ) - 1) );
										range = current;
										return result;
									} catch( boost::bad_lexical_cast const & ) { }
								}
							}
							return value_opt_t( );
						}

						template<typename Iterator>
						value_opt_t parse_value( Range<Iterator> & range );

						template<typename Iterator>
						boost::optional<object_value_item> parse_object_item( Range<Iterator> & range ) {
							auto current = range;							
							auto label = parse_string( current );
							if( !label || label->type( ) != typeid( std::string ) ) {
								return boost::optional<object_value_item>( );
							}
							
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
							return std::make_pair< std::string, value_opt_t >( std::move( boost::get<std::string>( *label ) ), std::move( value ) );
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
								result.members.push_back( *item );
								skip_ws( current );
								if( !is_equal( current.first, ',' ) ) {
									break;
								}
								current.move_next( );
							} while( !at_end( current ) );
							if( !is_equal( current.first, '}' ) ) {
								return value_opt_t( );
							}
							range = current;
							return value_opt_t( std::move( result ) );
						}

						template<typename Iterator>
						value_opt_t parse_array( Range<Iterator>& range ) {
							auto current = range;
							if( !is_equal( current.first, '[' ) ) {
								return value_opt_t( );
							}
							array_value results;
							do {
								skip_ws( current );
								auto item = parse_value( current );
								if( !item ) {
									return value_opt_t( );
								}
								results.items.push_back( std::move( item ) );
								skip_ws( current );
							} while( !current.at_end( ) && is_equal( current.first, ',' ) );
							if( !is_equal( current.first, ']' ) ) {
								return value_opt_t( );
							}
							range = current;
							return value_opt_t( results );
						}

						template<typename Iterator>
						value_opt_t parse_value( Range<Iterator>& range ) {
							auto current = range;
							skip_ws( current );
							switch( *current.first ) {
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

			}	// namespace json
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw

