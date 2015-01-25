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
#include <functional>
#include <memory>
#include <type_traits>

#include "base_event_emitter.h"
#include "base_work_queue.h"
#include "daw_json_link.h"
#include "lib_http_request.h"
#include "lib_http_site.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				namespace impl {
					template<typename ResultType, typename Argument> class HttpWebServiceImpl;
				}

				// TODO: build trait that allows for types that are jsonlink like or have a value_to_json/json_to_value overload
				template<typename ResultType, typename Argument> //, typename std::enable_if<daw::traits::is_mixed_from<daw::json::JsonLink, ResultType>::value && daw::traits::is_mixed_from<daw::json::JsonLink, Argument>::value, long>::type = 0>
				using HttpWebService = std::shared_ptr < impl::HttpWebServiceImpl < ResultType, Argument > > ;

				namespace impl {
					template<typename ResultType, typename Argument>
					class HttpWebServiceImpl: public daw::nodepp::base::enable_shared<HttpWebServiceImpl<ResultType, Argument>>, public daw::nodepp::base::StandardEvents < HttpWebServiceImpl<ResultType, Argument> > {
						// 						static_assert < std::is_base_of<JsonLink<ResultType>, ResultType>::value, "ResultType must derive from JsonLink<ResultType>" );
						// 						static_assert < std::is_base_of<JsonLink<Argument...>, ResultType>::value, "Argument must derive from JsonLink<Argument>" );
					public:
						HttpWebServiceImpl( daw::nodepp::lib::http::HttpClientRequestMethod method, boost::string_ref base_path, std::function < ResultType( Argument const & )> handler, bool synchronous = false ) { }

						template<typename T>
						T decode( boost::string_ref json_text );

						HttpWebServiceImpl& connect( HttpSite & site ) {
							auto self = get_weak_ptr( );
							site->delegate_to( "exit", self, "exit" ).
								delegate_to( "error", self, "error" ).
								on_requests_for( method, base_path, [self, handler, synchronous]( daw::nodepp::lib::http::HttpClientRequest request, daw::nodepp::lib::http::HttpServerResponse response ) {
								switch( request->request.method ) {
								case daw::nodepp::lib::http::HttpClientRequestMethod::Get:

								case daw::nodepp::lib::http::HttpClientRequestMethod::Post:
								default:
									throw std::runtime_error( "Web Service HTTP Method not implemented" );
								}
							} );
							return *this;
						}

						//
					};	// class HttpWebService
				}

				template < typename ResultType, typename Argument> //, typename std::enable_if<daw::traits::is_mixed_from<daw::json::JsonLink, ResultType>::value && daw::traits::is_mixed_from<daw::json::JsonLink, Argument>::value, long>::type = 0>
				HttpWebService<ResultType, Argument> create_web_service( daw::nodepp::lib::http::HttpClientRequestMethod method, boost::string_ref base_path, std::function < ResultType( Argument const & )> handler, bool synchronous = false ) {
					//
					return std::make_shared<impl::HttpWebServiceImpl<ResultType, Argument>>( method, base_path, handler, synchronous );
				}
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
