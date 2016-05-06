// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Darrell Wright
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
#include <daw/parse_json/daw_json_link.h>
#include "lib_http_request.h"
#include "lib_http_site.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				namespace impl {
					template<typename ResultType, typename... Argument> class HttpWebServiceImpl;
				}

				namespace impl {
					template<typename ResultType, typename... Argument>
					class HttpWebServiceImpl final: public daw::nodepp::base::enable_shared<HttpWebServiceImpl<ResultType, Argument...>>, public daw::nodepp::base::StandardEvents <HttpWebServiceImpl<ResultType, Argument...>> {
						static_assert(sizeof...(Argument) <= 1, "Either 1 or 0 arguments are accepted");
						// 						static_assert <std::is_base_of<JsonLink<ResultType>, ResultType>::value, "ResultType must derive from JsonLink<ResultType>" );
						// 						static_assert <std::is_base_of<JsonLink<Argument...>, ResultType>::value, "Argument must derive from JsonLink<Argument>" );
						daw::nodepp::lib::http::HttpClientRequestMethod m_method;
						std::string m_base_path;
						std::function <ResultType( Argument const & ... )> m_handler;
						bool m_synchronous;
					public:
						HttpWebServiceImpl( ) = delete;
						HttpWebServiceImpl( daw::nodepp::lib::http::HttpClientRequestMethod method, boost::string_ref base_path, std::function <ResultType( Argument const & ... )> handler, bool synchronous = false, daw::nodepp::base::EventEmitter emitter = daw::nodepp::base::create_event_emitter( ) ):
							daw::nodepp::base::StandardEvents <HttpWebServiceImpl<ResultType, Argument...>>( std::move( emitter ) ),
							m_method( std::move( method ) ),
							m_base_path( base_path.to_string( ) ),
							m_handler( std::move( handler ) ),
							m_synchronous( std::move( synchronous ) ) { }
						
						HttpWebServiceImpl( HttpWebServiceImpl const & ) = default;
						HttpWebServiceImpl( HttpWebServiceImpl && ) = default;
						HttpWebServiceImpl & operator=( HttpWebServiceImpl const & ) = default;
						HttpWebServiceImpl & operator=( HttpWebServiceImpl && ) = default;

						template<typename T>
						T decode( boost::string_ref json_text );

						HttpWebServiceImpl& connect( HttpSite & site ) {
							auto self = this->get_weak_ptr( );
							site->delegate_to( "exit", self, "exit" ).
								delegate_to( "error", self, "error" ).
								on_requests_for( m_method, m_base_path, [self]( daw::nodepp::lib::http::HttpClientRequest request, daw::nodepp::lib::http::HttpServerResponse response ) {
								switch( request->request_line.method ) {
								case daw::nodepp::lib::http::HttpClientRequestMethod::Get:
								{
									daw::json::impl::object_value obj;
									//auto const & query = request->request_line.url.query;
									if( sizeof...(Argument)> 0 ) {
										if( auto const & query = request->request_line.url.query ) {

										} else if( request->body ) {
										}
									} else if( !self.expired( ) ) {
										auto myself = self.lock( );
										auto result = myself->m_handler( 5 /*daw*/ );
									}

									break;
								}
								case daw::nodepp::lib::http::HttpClientRequestMethod::Any:
								case daw::nodepp::lib::http::HttpClientRequestMethod::Connect:
								case daw::nodepp::lib::http::HttpClientRequestMethod::Delete:
								case daw::nodepp::lib::http::HttpClientRequestMethod::Head:
								case daw::nodepp::lib::http::HttpClientRequestMethod::Options:
								case daw::nodepp::lib::http::HttpClientRequestMethod::Post:
								case daw::nodepp::lib::http::HttpClientRequestMethod::Put:
								case daw::nodepp::lib::http::HttpClientRequestMethod::Trace:
								default:
									throw std::runtime_error( "Web Service HTTP Method not implemented" );
								}
							} );
							return *this;
						}

						//
					};	// class HttpWebService
				}

				// TODO: build trait that allows for types that are jsonlink like or have a value_to_json/json_to_value overload
				template<typename ResultType, typename Argument> //, typename std::enable_if_t<daw::traits::is_mixed_from<daw::json::JsonLink, ResultType>::value && daw::traits::is_mixed_from<daw::json::JsonLink, Argument>::value, long> = 0>
				using HttpWebService = std::shared_ptr <impl::HttpWebServiceImpl <ResultType, Argument>>;

				template <typename ResultType, typename Argument> //, typename std::enable_if_t<daw::traits::is_mixed_from<daw::json::JsonLink, ResultType>::value && daw::traits::is_mixed_from<daw::json::JsonLink, Argument>::value, long> = 0>
				HttpWebService<ResultType, Argument> create_web_service( daw::nodepp::lib::http::HttpClientRequestMethod method, boost::string_ref base_path, std::function <ResultType( Argument const & )> handler, bool synchronous = false ) {
					//
					return std::make_shared<impl::HttpWebServiceImpl<ResultType, Argument>>( method, base_path, handler, synchronous );
				}
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw

