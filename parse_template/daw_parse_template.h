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
#include <boost/utility/string_ref.hpp>
#include <unordered_map>
#include <vector>
#include <future>
#include <string>
#include <numeric>
#include <ctime>
#include <iomanip>
#include <cassert>

#include "daw_char_range.h"

namespace daw {
	namespace parse_template {
		class ParseTemplate;

		namespace impl {
			struct CallbackMap {
				using iterator = typename boost::string_ref::iterator;
				enum class CallbackTypes { Normal, Date, Time, DateFormat, TimeFormat, Repeat, Unknown };
				std::vector<iterator> beginnings;
				std::vector<iterator> endings;
				std::vector<CallbackTypes> types;
				std::vector<std::vector<std::string>> arguments;

				size_t size( ) const;
				void clear( );
				void add( iterator beginning, iterator ending, CallbackTypes callback_type );
				void add( iterator beginning, iterator ending, CallbackTypes callback_type, std::vector<std::string> argument );
				void sort( );

				struct helpers {
					template<typename Container, typename Compare>
					static std::vector<std::size_t> sort_permutation( Container const & vec, Compare compare ) {
						std::vector<std::size_t> p( vec.size( ) );
						std::iota( p.begin( ), p.end( ), 0 );
						std::sort( p.begin( ), p.end( ), [&]( std::size_t i, std::size_t j ) { return compare( vec[i], vec[j] ); } );
						return p;
					}

					template<typename Container>
					static void apply_permutation( Container & vec, std::vector<std::size_t> const & p ) {
						std::vector<typename Container::value_type> sorted_vec( p.size( ) );
						std::transform( p.begin( ), p.end( ), sorted_vec.begin( ), [&]( std::size_t i ) { return vec[i]; } );
						vec = std::move( sorted_vec );
					}

					static void wait_for_all( std::initializer_list<std::future<void>> items ) {
						for( auto const & item : items ) {
							item.wait( );
						}
					}

				};	// struct helpers
			}; // struct CallbackMap

			struct CB {
				std::function<std::string( )> cb_normal;
				std::function<std::vector<std::string>( )> cb_repeat;
			};	// struct CB

			template<typename T>
			T const & make_const( T const & value ) {
				return value;
			}

		} // namespace impl

		class ParseTemplate {
			std::unordered_map<std::string, impl::CB> m_callbacks;
			range::CharRange m_template;
			std::unique_ptr<impl::CallbackMap> m_callback_map;
		public:
			ParseTemplate( ) = default;	
			~ParseTemplate( ) = default;
			ParseTemplate( ParseTemplate const & other );
			ParseTemplate( ParseTemplate && ) = default;
			ParseTemplate & operator=( ParseTemplate const & rhs );
			ParseTemplate & operator=( ParseTemplate && ) = default;
			ParseTemplate( range::CharRange template_string );
			void generate_callbacks( );
			std::string process_template_to_string();		

			template<typename Stream>
			void process_template( Stream & out_stream ) {
				auto show_string = []( auto & stream, auto first, auto const last ) {
					for( ; first != last; ++first ) {
						stream << *first;
					}
					return first;
				};

				std::string dte_format = "%x";
				std::string tm_format = "%X";

				auto pos = m_template.begin( );

				for( size_t n = 0; n < m_callback_map->size( ); ++n ) {
					pos = show_string( out_stream, pos, m_callback_map->beginnings[n] );
					switch( m_callback_map->types[n] ) {
					case impl::CallbackMap::CallbackTypes::Normal:
					{
						if( m_callback_map->arguments[n].size( ) != 1 ) {
							out_stream << "Error, invalid arguments, expected 1, at position " << std::distance( m_template.begin( ), m_callback_map->beginnings[n] ) << "\n";
							break;
						}
						auto const & cb_name = m_callback_map->arguments[n][0];
						if( !cb_name.empty( ) && callback_exists( cb_name ) && m_callbacks[cb_name].cb_normal ) {
							out_stream << m_callbacks[cb_name].cb_normal( );
						}
					}
					break;
					case impl::CallbackMap::CallbackTypes::Date:
					{
						std::time_t t = std::time( nullptr );
						std::tm tm = *std::localtime( &t );
						out_stream << std::put_time( &tm, dte_format.c_str( ) );
					}
					break;
					case impl::CallbackMap::CallbackTypes::Time:
					{
						std::time_t t = std::time( nullptr );
						std::tm tm = *std::localtime( &t );
						out_stream << std::put_time( &tm, tm_format.c_str( ) );
					}
					break;
					case impl::CallbackMap::CallbackTypes::DateFormat:
						if( m_callback_map->arguments[n].size( ) != 1 ) {
							out_stream << "Error, invalid arguments, expected 1, at position " << std::distance( m_template.begin( ), m_callback_map->beginnings[n] ) << "\n";
							break;
						}
						dte_format = m_callback_map->arguments[n][0];
						break;
					case impl::CallbackMap::CallbackTypes::TimeFormat:
						if( m_callback_map->arguments[n].size( ) != 1 ) {
							out_stream << "Error, invalid arguments, expected 1, at position " << std::distance( m_template.begin( ), m_callback_map->beginnings[n] ) << "\n";
							break;
						}
						tm_format = m_callback_map->arguments[n][0];
						break;
					case impl::CallbackMap::CallbackTypes::Repeat:
					{
						if( m_callback_map->arguments[n].empty( ) ) {
							out_stream << "Error, invalid arguments, expected 1, at position " << std::distance( m_template.begin( ), m_callback_map->beginnings[n] ) << "\n";
							break;
						}
						auto const & cb_name = m_callback_map->arguments[n][0];

						if( !cb_name.empty( ) && callback_exists( cb_name ) && m_callbacks[cb_name].cb_repeat ) {
							std::string prefix = "";
							std::string postfix = "";
							if( m_callback_map->arguments[n].size( ) == 3 ) {
								prefix = m_callback_map->arguments[n][1];
								postfix = m_callback_map->arguments[n][2];
							}
							auto tmp = m_callbacks[cb_name].cb_repeat( );
							auto count = tmp.size( );
							for( auto const & line : tmp ) {
								out_stream << prefix << line << postfix;
								if( count-- > 1 ) {
									out_stream << "\n";
								}
							}
						}
					}					
					break;
					case impl::CallbackMap::CallbackTypes::Unknown:
						out_stream << "Error, unknown tag at position " << std::distance( m_template.begin( ), m_callback_map->beginnings[n] ) << "\n";
						break;
					}
					pos = m_callback_map->endings[n];
				}
				show_string( out_stream, pos, m_template.end( ) );
			}

			std::vector<std::string> list_callbacks( ) const;
			void callback_remove( boost::string_ref callback_name );
			bool callback_exists( boost::string_ref callback_name ) const;
			void add_callback_impl(boost::string_ref callback_name, std::function<std::string()> callback);
			void add_callback_impl(boost::string_ref callback_name, std::function<std::vector<std::string>( )> callback);

			template<typename CallbackFunction>
			void add_callback( boost::string_ref callback_name, CallbackFunction callback ) {
				using result_t = typename std::result_of<CallbackFunction( )>::type;
				std::function<result_t( )> cb = callback;
				add_callback_impl( callback_name, cb );
			}

		};	// class ParseTemplate
	}	// namespace parse_template
}	// namespace daw
