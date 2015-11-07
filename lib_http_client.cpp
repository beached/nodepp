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

#include <cstdlib>
#include <memory>
#include <iostream>

#include "lib_http_client.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				HttpClient create_http_client( daw::nodepp::base::EventEmitter emitter ) {
					return HttpClient { nullptr };
				}

				HttpClientConnection create_http_client_connection( daw::nodepp::base::EventEmitter emitter ) {
					return HttpClientConnection { nullptr };
				}

				namespace impl {
					HttpClientImpl & HttpClientImpl::on_connection( std::function<void( HttpClientConnection )> listener ) {
						return *this;
					}

					void HttpClientImpl::request( boost::string_ref scheme, boost::string_ref host, uint16_t port, daw::nodepp::lib::http::HttpClientRequest request ) {
						m_client->delegate_to( "on_connection", this->get_weak_ptr( ), "on_connection" );
						m_client->connect( host, port );
					}

					daw::nodepp::base::EventEmitter& HttpClientImpl::emitter( ) {
						return m_emitter;
					}

					HttpClientConnectionImpl& HttpClientConnectionImpl::on_response_returned( std::function<void( daw::nodepp::lib::http::HttpServerResponse )> listener ) {
						return *this;
					}

					HttpClientConnectionImpl& HttpClientConnectionImpl::on_next_response_returned( std::function<void( daw::nodepp::lib::http::HttpServerResponse )> listener ) {
						return *this;
					}

					HttpClientConnectionImpl & HttpClientConnectionImpl::on_closed( std::function<void( )> listener ) {
						return *this;
					}
				}	//namespace impl
			}	// namespace http
		} // namespace lib
	}	// namespace nodepp
}	// namespace daw