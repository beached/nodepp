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
#include <memory>
#include <type_traits>

#include "base_event_emitter.h"
#include "daw_json_link.h"
#include "lib_http_request.h"
#include "lib_http_server.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				namespace impl {
					template<typename ResultType, typename... Arguments> class HttpWebService;
				}

				template<typename ResultType, typename... Arguments> using HttpWebService = std::shared_ptr < impl::HttpWebServiceImpl < ResultType, Arguments... > > ;

				namespace impl {
					template<typename ResultType, typename... Arguments>
					class HttpWebServiceImpl: public daw::nodepp::base::enable_shared<HttpWebService>, public public daw::nodepp::base::StandardEvents < HttpWebService > {
						static_assert < std::is_base_of<JsonLink<ResultType>, ResultType>::value, "ResultType must derive from JsonLink<ResultType>" );
						static_assert < std::is_base_of<JsonLink<Arguments...>, ResultType>::value, "Arguments must derive from JsonLink<Arguments>" );

						HttpWebServiceImpl( boost::string_ref base_path, daw::nodepp::lib::http::HttpClientRequestMethod method, std::function < JsonLink<ResultType>( JsonLink<Arguments>... ) ) { }

						HttpWebServiceImpl& connect( HttpServer & server );
						//
					};	// class HttpWebService
				}

				template < typename ResultType, typename... Arguments, typename sizeof( decltype(value_to_json( ResultType )) ) != 0 && sizeof( decltype(value_to_json( Arguments )) ) != 0 >
				HttpWebService create_web_service( ) { }
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
