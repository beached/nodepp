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

#include <boost/optional.hpp>
#include <boost/utility/string_ref.hpp>
#include <boost/variant.hpp>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace daw {
	namespace nodepp {
		namespace base {
			namespace json {
				namespace impl {										
					struct null_value { };
					struct object_value;
					struct array_value;
					using value_t = boost::variant < int64_t, double, std::string, bool, null_value, array_value, object_value > ;
					using value_opt_t = boost::optional < value_t > ;
					
					struct array_value {
						std::vector<value_opt_t> items;
					};

					using object_value_item = std::pair < value_t, value_opt_t > ;

					struct object_value {
						std::vector<object_value_item> members;
						using iterator = std::vector<object_value_item>::iterator;
						iterator find( boost::string_ref const key );
						iterator begin( );
						iterator end( );
					};

				}	// namespace impl
				using json_obj = std::shared_ptr < impl::value_t > ;

				json_obj parse_json( boost::string_ref const json_text );


			}	// namespace json
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw

