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

#include <algorithm>
#include <future>
#include <sstream>
#include <ctime>
#include <iomanip>

#include "daw_parse_template.h"

namespace daw {
	namespace parse_template {
		namespace impl {
			size_t CallbackMap::size( ) const {
				return beginnings.size( );
			}

			void CallbackMap::clear( ) {
				beginnings.clear( );
				endings.clear( );
				types.clear( );
				arguments.clear( );
			}

			void CallbackMap::add( CallbackMap::iterator beginning, CallbackMap::iterator ending, CallbackMap::CallbackTypes callback_type ) {
				beginnings.push_back( beginning );
				endings.push_back( ending );
				types.push_back( callback_type );
				arguments.emplace_back( "" );
			}

			void CallbackMap::add( CallbackMap::iterator beginning, CallbackMap::iterator ending, CallbackMap::CallbackTypes callback_type, std::string argument ) {
				beginnings.push_back( beginning );
				endings.push_back( ending );
				types.push_back( callback_type );
				arguments.push_back( std::move( argument ) );
			}

			void CallbackMap::sort( ) {
				auto perm = helpers::sort_permutation( beginnings, []( auto const & A, auto const & B ) { return A < B; } );
				helpers::wait_for_all( {
					std::async( std::launch::async, [&]( ) { helpers::apply_permutation( beginnings, perm ); } ),
					std::async( std::launch::async, [&]( ) { helpers::apply_permutation( endings, perm ); } ),
									   std::async( std::launch::async, [&]( ) { helpers::apply_permutation( types, perm ); } ),
									   std::async( std::launch::async, [&]( ) { helpers::apply_permutation( arguments, perm ); } )
				} );
			}
		}
		ParseTemplate::ParseTemplate( ParseTemplate const & other ):
			m_callbacks( other.m_callbacks ),
			m_template( other.m_template ),
			m_callback_map( std::make_unique<impl::CallbackMap>( *other.m_callback_map ) ) {

			generate_callbacks( );
		}

		ParseTemplate& ParseTemplate::operator=( ParseTemplate const & rhs ) {
			if( this != &rhs ) {
				m_callbacks = rhs.m_callbacks;
				m_template = rhs.m_template;
				m_callback_map = std::make_unique<impl::CallbackMap>( *rhs.m_callback_map );
			}
			return *this;
		}

		ParseTemplate::ParseTemplate( boost::string_ref template_string ):
			m_callbacks( ),
			m_template( template_string ),
			m_callback_map( std::make_unique<impl::CallbackMap>( ) ) {

			generate_callbacks( );
		}

		namespace {
			template<typename Iterator>
			bool is_equal_shortest( Iterator first1, Iterator last1, Iterator first2, Iterator last2 ) {
				// compare the two ranges to the length of the shortest
				while( first1 != last1 && first2 != last2 ) {
					if( *first1 != *first2 ) {
						return false;
					}
					++first1;
					++first2;
				}
				return true;
			}

			template<typename Iterator>
			Iterator find_string( Iterator first, Iterator last, boost::string_ref value ) {
				auto result_it = std::find( first, last, *value.begin( ) );
				while( result_it != last ) {

					if( is_equal_shortest( result_it, last, value.begin( ), value.end( ) ) ) {
						return result_it;
					}
					++result_it;
				}
				return result_it;
			}
		}

		void ParseTemplate::generate_callbacks( ) {
			assert( m_callback_map );
			m_callback_map->clear( );

			auto parse_tag_type = [&]( auto first, auto const & last ) {
				auto result = impl::CallbackMap::CallbackTypes::Unknown;
				switch( *first ) {
				case '=':
					result = impl::CallbackMap::CallbackTypes::Normal;
					break;
				case 'r':
					++first;
					if( find_string( first, last, "epeat" ) == last ) {
						break;
					}
					result = impl::CallbackMap::CallbackTypes::Repeat;
					break;
				case 'd':
					++first;
					if( find_string( first, last, "ate" ) != last ) {
						first += 3;
						if( find_string( first, last, "_format" ) != last ) {
							result = impl::CallbackMap::CallbackTypes::DateFormat;
							break;
						}
						result = impl::CallbackMap::CallbackTypes::Date;
					}
					break;
				case 't':
					++first;
					if( find_string( first, last, "ime" ) != last ) {
						first += 3;
						if( find_string( first, last, "_format" ) != last ) {
							result = impl::CallbackMap::CallbackTypes::TimeFormat;
							break;
						}
						result = impl::CallbackMap::CallbackTypes::Time;
					}
					break;
				}
				return result;
			};

			auto get_callback = [&]( size_t callback_map_pos ) {
				std::string result = "";
				switch( m_callback_map->types[callback_map_pos] ) {
				case impl::CallbackMap::CallbackTypes::Normal:
				{
					auto first = m_callback_map->beginnings[callback_map_pos] + 2;
					// TODO find quotes
					auto name = boost::string_ref { first, static_cast<size_t>(std::distance( first, m_callback_map->endings[callback_map_pos] ) - 1) };
					if( callback_exists( name ) ) {
						result = name.to_string( );
					}
				}
				break;
				case impl::CallbackMap::CallbackTypes::Repeat:
				{
					auto first = m_callback_map->beginnings[callback_map_pos] + 8;	// <%repeat=" legnth of repeat="
					auto name = boost::string_ref { first, static_cast<size_t>(std::distance( first, m_callback_map->endings[callback_map_pos] )) };
					if( callback_exists( name ) ) {
						result = name.to_string( );
					}
				}
				break;
				case impl::CallbackMap::CallbackTypes::Date: break;
				case impl::CallbackMap::CallbackTypes::Time: break;
				case impl::CallbackMap::CallbackTypes::DateFormat: break;
				case impl::CallbackMap::CallbackTypes::TimeFormat: break;
				case impl::CallbackMap::CallbackTypes::Unknown: break;
				case impl::CallbackMap::CallbackTypes::String: break;
				default: break;
				}

				return result;
			};

			auto find_tags = [&]( auto first, auto const & last, boost::string_ref open_tag, boost::string_ref close_tag ) {
				while( first != last ) {
					first = find_string( first, last, open_tag );
					if( first == last ) {
						break;
					}
					first += open_tag.size( );
					auto open_it = first;
					first = find_string( first, last, close_tag );
					if( first == last ) {
						break;
					}
					first += close_tag.size( );
					auto tag_type = parse_tag_type( open_it, first );
					m_callback_map->add( open_it, first, tag_type );
				}
			};

			auto add_strings = [&]( auto first, auto const & last ) {
				for( size_t n = 0; n < m_callback_map->size( ); ++n ) {
					if( first + 2 != m_callback_map->beginnings[n] ) {
						m_callback_map->add( first, first + (std::distance( first, m_callback_map->beginnings[n] ) - 2), impl::CallbackMap::CallbackTypes::String );
					}
					first = m_callback_map->endings[n];
				}
			};

			find_tags( m_template.begin( ), m_template.end( ), "<%", "%>" );
			add_strings( m_template.begin( ), m_template.end( ) );
			m_callback_map->sort( );
			for( size_t n = 0; n < m_callback_map->size( ); ++n ) {
				m_callback_map->arguments[n] = get_callback( n );
			}
		}

		std::string ParseTemplate::process_template( ) {
			std::stringstream ss;
			std::string dte_format = "%x";
			std::string tm_format = "%X";
			for( size_t n = 0; n < m_callback_map->size( ); ++n ) {
				switch( m_callback_map->types[n] ) {
				case impl::CallbackMap::CallbackTypes::Normal:
				{
					auto const & cb_name = m_callback_map->arguments[n];
					if( !cb_name.empty( ) && callback_exists( cb_name ) ) {
						std::string tmp;
						m_callbacks[cb_name]( &tmp );
						ss << tmp;
					}
				}
				break;
				case impl::CallbackMap::CallbackTypes::Date:
				{
					std::time_t t = std::time( nullptr );
					std::tm tm = *std::localtime( &t );
					ss << std::put_time( &tm, dte_format.c_str( ) );
				}
				break;
				case impl::CallbackMap::CallbackTypes::Time:
				{
					std::time_t t = std::time( nullptr );
					std::tm tm = *std::localtime( &t );
					ss << std::put_time( &tm, tm_format.c_str( ) );
				}
				break;
				case impl::CallbackMap::CallbackTypes::DateFormat:
					dte_format = m_callback_map->arguments[n];
					break;
				case impl::CallbackMap::CallbackTypes::TimeFormat:
					tm_format = m_callback_map->arguments[n];
					break;
				case impl::CallbackMap::CallbackTypes::Repeat:
				{
					auto const & cb_name = m_callback_map->arguments[n];
					if( !cb_name.empty( ) && callback_exists( cb_name ) ) {
						std::vector<std::string> tmp;
						m_callbacks[cb_name]( &tmp );
						for( auto const & line : tmp ) {
							ss << line << "\n";
						}
					}
				}
				break;
				case impl::CallbackMap::CallbackTypes::String:
					ss << (boost::string_ref { m_callback_map->beginnings[n], static_cast<size_t>(std::distance( m_callback_map->beginnings[n], m_callback_map->endings[n] )) });
					break;
				case impl::CallbackMap::CallbackTypes::Unknown:
					ss << "Error, unknown tag at position " << std::distance( m_template.begin( ), m_callback_map->beginnings[n] ) << "\n";
					break;
				}
			}
			return ss.str( );
		}

		std::vector<std::string> ParseTemplate::list_callbacks( ) const {
			std::vector<std::string> result;
			result.reserve( m_callbacks.size( ) );

			std::transform( m_callbacks.begin( ), m_callbacks.end( ), std::back_inserter( result ), []( typename decltype(m_callbacks)::value_type item ) {
				return item.first;
			} );

			return result;
		}

		void ParseTemplate::callback_remove( boost::string_ref callback_name ) {
			m_callbacks.erase( callback_name.to_string( ) );
		}

		bool ParseTemplate::callback_exists( boost::string_ref callback_name ) const {
			return m_callbacks.count( callback_name.to_string( ) ) != 0;
		}

	}	// namespace parse_template
}	// namespace daw

