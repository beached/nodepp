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

#include <boost/optional.hpp>
#include <boost/utility/string_ref.hpp>
#include <cstdint>
#include <ostream>

#include "base_event_emitter.h"
#include "base_service_handle.h"
#include "lib_http_url.h"
#include "lib_http_server_response.h"
#include "lib_http_client_connection_options.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				namespace impl {
					class HttpClientImpl;
					class HttpClientConnectionImpl;
				}

				class HttpClientResponseMessage { };

				using HttpClient = std::shared_ptr<impl::HttpClientImpl>;
				HttpClient create_http_client( daw::nodepp::base::EventEmitter emitter = daw::nodepp::base::create_event_emitter( ) );

				using HttpClientConnection = std::shared_ptr<impl::HttpClientConnectionImpl>;
				HttpClientConnection create_http_client_connection( daw::nodepp::lib::net::NetSocketStream socket, daw::nodepp::base::EventEmitter emitter = daw::nodepp::base::create_event_emitter( ) );

				namespace impl {
					//////////////////////////////////////////////////////////////////////////
					// Summary:		An HTTP Client class
					// Requires:
					class HttpClientImpl final: public daw::nodepp::base::enable_shared<HttpClientImpl>, public daw::nodepp::base::StandardEvents<HttpClientImpl> {
						daw::nodepp::lib::net::NetSocketStream m_client;

					public:
						explicit HttpClientImpl( daw::nodepp::base::EventEmitter emitter );
						HttpClientImpl( ) = delete;
						HttpClientImpl( HttpClientImpl const & ) = default;
						HttpClientImpl( HttpClientImpl && ) = default;
						HttpClientImpl& operator=( HttpClientImpl const & ) = default;
						HttpClientImpl& operator=( HttpClientImpl && ) = default;
						~HttpClientImpl( );

						void request( std::string scheme, std::string host, uint16_t port, daw::nodepp::lib::http::HttpClientRequest request );
						HttpClientImpl & on_connection( std::function<void( HttpClientConnection )> listener );
					};	// class HttpClientImpl

					class HttpClientConnectionImpl final: public daw::nodepp::base::enable_shared<HttpClientConnectionImpl>, public daw::nodepp::base::StandardEvents<HttpClientConnectionImpl> {
						daw::nodepp::lib::net::NetSocketStream m_socket;

					public:
						HttpClientConnectionImpl( ) = default;
						~HttpClientConnectionImpl( );
						HttpClientConnectionImpl( HttpClientConnectionImpl const & ) = default;
						HttpClientConnectionImpl( HttpClientConnectionImpl && ) = default;
						HttpClientConnectionImpl & operator=( HttpClientConnectionImpl const & ) = default;
						HttpClientConnectionImpl & operator=( HttpClientConnectionImpl && ) = default;

						HttpClientConnectionImpl( daw::nodepp::lib::net::NetSocketStream socket, daw::nodepp::base::EventEmitter emitter );

						HttpClientConnectionImpl & on_response_returned( std::function<void( daw::nodepp::lib::http::HttpServerResponse )> listener );
						HttpClientConnectionImpl & on_next_response_returned( std::function<void( daw::nodepp::lib::http::HttpServerResponse )> listener );
						HttpClientConnectionImpl & on_closed( std::function<void( )> listener );	// Only once as it is called on the way out
					};	// HttpClientConnectionImpl
				}	//namespace impl
				using HttpClientConnection = std::shared_ptr<impl::HttpClientConnectionImpl>;

				void get( boost::string_ref url_string, std::initializer_list<std::pair<std::string, HttpClientConnectionOptions::value_type>> options, std::function<void( HttpClientResponseMessage )> on_completion );

			}	// namespace http
		} // namespace lib
	}	// namespace nodepp
}	// namespace daw

