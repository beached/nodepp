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

#include "base_callback.h"

	namespace daw {
		namespace parse_template {
			namespace impl {
				struct CallbackMap;
			}
		class ParseTemplate {
			std::unordered_map<std::string, daw::nodepp::base::Callback> m_callbacks;
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
			template<typename Function>
			void add_callback( boost::string_ref callback_name, Function callback ) {
				m_callbacks[callback_name.to_string( )] = callback;
			}
		};
	}	// namespace parse_template
}	// namespace daw
