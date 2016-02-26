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
#include "base_callback.h"

namespace daw {
	namespace parse_template {
		namespace impl {
			struct CallbackMap {
				using iterator = typename boost::string_ref::iterator;
				enum class CallbackTypes { Normal, Date, Time, DateFormat, TimeFormat, Repeat, String, Unknown };
				std::vector<iterator> beginnings;
				std::vector<iterator> endings;
				std::vector<CallbackTypes> types;
				std::vector<std::string> arguments;

				size_t size( ) const;
				void clear( );
				void add( iterator beginning, iterator ending, CallbackTypes callback_type );
				void add( iterator beginning, iterator ending, CallbackTypes callback_type, std::string argument );
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

		} // namespace impl

		class ParseTemplate {
			std::unordered_map<std::string, impl::CB> m_callbacks;
			boost::string_ref m_template;
			std::unique_ptr<impl::CallbackMap> m_callback_map;
		public:
			ParseTemplate( ) = delete;	
			~ParseTemplate( ) = default;
			ParseTemplate( ParseTemplate const & other );
			ParseTemplate( ParseTemplate && ) = default;
			ParseTemplate & operator=( ParseTemplate const & rhs );
			ParseTemplate & operator=( ParseTemplate && ) = default;
			ParseTemplate( boost::string_ref template_string );
			void generate_callbacks( );
			std::string process_template( );
			std::vector<std::string> list_callbacks( ) const;
			void callback_remove( boost::string_ref callback_name );
			bool callback_exists( boost::string_ref callback_name ) const;
			void add_callback_impl(boost::string_ref callback_name, std::function<std::string()> callback);
			void add_callback_impl(boost::string_ref callback_name, std::function<std::vector<std::string>()> callback);

			template<typename CallbackFunction>
			void add_callback( boost::string_ref callback_name, CallbackFunction callback ) {
				using result_t = std::result_of<CallbackFunction( )>::type;
				std::function<result_t( )> cb = callback;
				add_callback_impl( callback_name, cb );
			}

		};	// class ParseTemplate
	}	// namespace parse_template
}	// namespace daw
