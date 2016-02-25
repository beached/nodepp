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
#include "daw_parse_template.h"


namespace daw {
	namespace parse_template {
		namespace impl {
			struct CallbackMap {
				using iterator = typename boost::string_ref::iterator;
				enum class CallbackTypes { Normal, Date, Time, DateFormat, TimeFormat, Repeat };
				std::vector<iterator> beginnings;
				std::vector<iterator> endings;
				std::vector<CallbackTypes> types;
				std::vector<daw::nodepp::base::Callback*> callback_handles;

				void clear( ) {
					beginnings.clear( );
					endings.clear( );
					types.clear( );
					callback_handles.clear( );
				}

				void add( iterator beginning, iterator ending, CallbackTypes callback_type ) {
					beginnings.push_back( beginning );
					endings.push_back( ending );
					types.push_back( callback_type );
					callback_handles.emplace_back( );
				}

				void add( iterator beginning, iterator ending, CallbackTypes callback_type, daw::nodepp::base::Callback* callback_handle ) {
					beginnings.push_back( beginning );
					endings.push_back( ending );
					types.push_back( callback_type );
					callback_handles.push_back( callback_handle );
				}
			};
		}
		ParseTemplate::ParseTemplate( ParseTemplate const & other ):
			m_callbacks( other.m_callbacks ),
			m_template( other.m_template ),
			m_callback_map( std::make_unique<impl::CallbackMap>( *other.m_callback_map ) ) {

			generate_callbacks( );
		}

		ParseTemplate& ParseTemplate::operator=(ParseTemplate const & rhs ) {
			if( this != &rhs ) {
				m_callbacks = rhs.m_callbacks;
				m_template = rhs.m_template;
				m_callback_map = std::make_unique<impl::CallbackMap>( *rhs.m_callback_map );
			}
			return *this;
		}

		ParseTemplate::ParseTemplate(boost::string_ref template_string):
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

			auto current_it = m_template.begin( );

			while( (current_it = find_string( current_it, m_template.end( ), "<%" )) != m_template.end( ) ) {
				current_it += 2;
				if( *current_it == '=' ) {					
					// Normal Callback
					auto next_it = find_string( current_it, m_template.end( ), "%>" );
					if( next_it == m_template.end( ) ) {
						return;
					}
					m_callback_map->add( current_it, next_it, impl::CallbackMap::CallbackTypes::Normal );
					current_it = next_it;
				} else if( find_string( current_it, m_template.end( ), "date" ) != m_template.end( ) ) {
					current_it += 4;
					if( find_string( current_it, m_template.end( ), "_format=\"" ) != m_template.end( ) ) {
						current_it += 9;
						auto end_format_it = find_string( current_it, m_template.end( ), "\"" );
						if( end_format_it == m_template.end( ) ) {
							return;
						}


					} else {
						auto next_it = find_string( current_it, m_template.end( ), "%>" );
						if( next_it == m_template.end( ) ) {
							return;
						}
						m_callback_map->add( current_it, next_it, impl::CallbackMap::CallbackTypes::Date );
					}
				}
			}			
		}

		std::string ParseTemplate::process_template( ) {

			return "";
		}

		std::vector<std::string> ParseTemplate::list_callbacks() const {
			std::vector<std::string> result;
			result.reserve( m_callbacks.size( ) );
			
			std::transform( m_callbacks.begin( ), m_callbacks.end( ), std::back_inserter( result ), []( typename decltype(m_callbacks)::value_type item ) {
				return item.first;
			} );

			return result;
		}

		void ParseTemplate::callback_remove(boost::string_ref callback_name) {
			m_callbacks.erase( callback_name.to_string( ) );
		}
	}	// namespace parse_template
}	// namespace daw

