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
#include <cstdint>
#include <iostream>
#include <ostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "base_types.h"
#include "base_event_emitter.h"
#include "parse_json/daw_json_link.h"
#include "lib_http_request.h"

namespace daw {
	namespace nodepp {
		namespace base {
			namespace json {
				struct JsonLink;
			}
		}
		namespace lib {
			namespace http {
				namespace impl {
					class HttpClientImpl;
				}
				using HttpClient = std::shared_ptr<impl::HttpClientImpl>;
				HttpClient create_http_client( daw::nodepp::base::EventEmitter emitter = daw::nodepp::base::create_event_emitter( ) );

				namespace impl {
					//////////////////////////////////////////////////////////////////////////
					// Summary:		An HTTP Client class
					// Requires:
					class HttpClientImpl: public daw::nodepp::base::enable_shared<HttpClientImpl>, public daw::nodepp::base::StandardEvents<HttpClientImpl> {
						daw::nodepp::base::EventEmitter m_emitter;

					public:
						friend daw::nodepp::lib::http::HttpClient daw::nodepp::lib::http::create_http_client( daw::nodepp::base::EventEmitter );

						HttpClientImpl( HttpClientImpl&& other );
						HttpClientImpl& operator=( HttpClientImpl const & ) = default;
						HttpClientImpl& operator=( HttpClientImpl && rhs );
						HttpClientImpl( HttpClientImpl const & ) = default;
						~HttpClientImpl( ) = default;

						daw::nodepp::base::EventEmitter& emitter( );
					};	// class HttpClientImpl
				}	//namespace impl
			}	// namespace http
		} // namespace lib
	}	// namespace nodepp
}	// namespace daw
