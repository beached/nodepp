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
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "base_event_emitter.h"
#include "lib_http_request.h"
#include "lib_http_server_response.h"
#include "lib_http_server.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				namespace impl {
					class HttpSiteImpl;
				}	// namespace impl

				using HttpSite = std::shared_ptr <impl::HttpSiteImpl>;

				HttpSite create_http_site( daw::nodepp::base::EventEmitter emitter = daw::nodepp::base::create_event_emitter( ) );
				HttpSite create_http_site( HttpServer server, daw::nodepp::base::EventEmitter emitter = daw::nodepp::base::create_event_emitter( ) );

				namespace impl {
					struct site_registration final {
						std::string host;	// * = any
						std::string path;	// postfixing with a * means match left(will mean)
						HttpClientRequestMethod method;
						std::function <void( daw::nodepp::lib::http::HttpClientRequest, daw::nodepp::lib::http::HttpServerResponse )> listener;

						site_registration( ) = default;
						~site_registration( ) = default;
						site_registration( site_registration const & ) = default;
						site_registration( site_registration && ) = default;
						site_registration& operator=( site_registration const & ) = default;
						site_registration& operator=( site_registration && ) = default;						

						bool operator==( site_registration const & rhs ) const;

						site_registration( boost::string_ref Host, boost::string_ref Path, daw::nodepp::lib::http::HttpClientRequestMethod Method );
						site_registration( boost::string_ref Host, boost::string_ref Path, daw::nodepp::lib::http::HttpClientRequestMethod Method, std::function <void( daw::nodepp::lib::http::HttpClientRequest, daw::nodepp::lib::http::HttpServerResponse )> Listener );
					};	// site_registration

					class HttpSiteImpl final: public daw::nodepp::base::enable_shared<HttpSiteImpl>, public daw::nodepp::base::StandardEvents <HttpSiteImpl> {
					public:
						using registered_pages_t = std::vector <site_registration>;
						using iterator = registered_pages_t::iterator;

					private:
						daw::nodepp::lib::http::HttpServer m_server;
						registered_pages_t m_registered_sites;
						std::unordered_map<uint16_t, std::function <void( HttpClientRequest, daw::nodepp::lib::http::HttpServerResponse, uint16_t )>> m_error_listeners;

						void sort_registered( );
						void default_page_error_listener( daw::nodepp::lib::http::HttpClientRequest request, daw::nodepp::lib::http::HttpServerResponse response, uint16_t error_no );
						void start( );

						explicit HttpSiteImpl( daw::nodepp::base::EventEmitter emitter );
						HttpSiteImpl( daw::nodepp::lib::http::HttpServer server, daw::nodepp::base::EventEmitter emitter );
					public:
						friend HttpSite daw::nodepp::lib::http::create_http_site( daw::nodepp::base::EventEmitter );
						friend HttpSite daw::nodepp::lib::http::create_http_site( daw::nodepp::lib::http::HttpServer, daw::nodepp::base::EventEmitter );
						
						~HttpSiteImpl( );
						HttpSiteImpl( HttpSiteImpl const & ) = delete;
						HttpSiteImpl( HttpSiteImpl&& ) = default;
						HttpSiteImpl& operator=( HttpSiteImpl const & ) = delete;
						HttpSiteImpl& operator=( HttpSiteImpl&& ) = default;
						
						//////////////////////////////////////////////////////////////////////////
						/// Summary:	Register a listener for a HTTP method and path on any
						///				host
						HttpSiteImpl& on_requests_for( daw::nodepp::lib::http::HttpClientRequestMethod method, std::string path, std::function<void( daw::nodepp::lib::http::HttpClientRequest, daw::nodepp::lib::http::HttpServerResponse )> listener );

						//////////////////////////////////////////////////////////////////////////
						/// Summary:	Register a listener for a HTTP method and path on a
						///				specific hostname
						HttpSiteImpl& on_requests_for( boost::string_ref hostname, daw::nodepp::lib::http::HttpClientRequestMethod method, std::string path, std::function<void( daw::nodepp::lib::http::HttpClientRequest, daw::nodepp::lib::http::HttpServerResponse )> listener );

						void remove_site( iterator item );

						iterator end( );
						iterator match_site( boost::string_ref host, boost::string_ref path, daw::nodepp::lib::http::HttpClientRequestMethod method );

						bool has_error_handler( uint16_t error_no );

						//////////////////////////////////////////////////////////////////////////
						// Summary:	Use the default error handler for HTTP errors. This is the
						//			default.
						HttpSiteImpl& clear_page_error_listeners( );

						//////////////////////////////////////////////////////////////////////////
						// Summary:	Create a generic error handler
						HttpSiteImpl& on_any_page_error( std::function <void( daw::nodepp::lib::http::HttpClientRequest, daw::nodepp::lib::http::HttpServerResponse, uint16_t error_no )> listener );

						//////////////////////////////////////////////////////////////////////////
						// Summary:	Use the default error handler for specific HTTP error.
						HttpSiteImpl& except_on_page_error( uint16_t error_no );

						//////////////////////////////////////////////////////////////////////////
						// Summary:	Specify a callback to handle a specific page error
						HttpSiteImpl& on_page_error( uint16_t error_no, std::function <void( daw::nodepp::lib::http::HttpClientRequest, daw::nodepp::lib::http::HttpServerResponse, uint16_t error_number )> listener );

						void emit_page_error( daw::nodepp::lib::http::HttpClientRequest request, daw::nodepp::lib::http::HttpServerResponse response, uint16_t error_no );
						void emit_listening( daw::nodepp::lib::net::EndPoint endpoint );

						HttpSiteImpl& on_listening( std::function<void( daw::nodepp::lib::net::EndPoint )> listener );
						HttpSiteImpl& listen_on( uint16_t port );
					};	// class HttpSiteImpl
				}	// namespace impl
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw

