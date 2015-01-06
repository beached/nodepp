#pragma once
///	The MIT License (MIT)
///	
///	Copyright (c) 2014-2015 Darrell Wright
///	
///	Permission is hereby granted, free of charge, to any person obtaining a copy
///	of this software and associated documentation files (the "Software"), to deal
///	in the Software without restriction, including without limitation the rights
///	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
///	copies of the Software, and to permit persons to whom the Software is
///	furnished to do so, subject to the following conditions:
///	
///	The above copyright notice and this permission notice shall be included in all
///	copies or substantial portions of the Software.
///	
///	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
///	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
///	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
///	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
///	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
///	SOFTWARE.
///

#include <boost/utility/string_ref.hpp>
#include <cstdint>
#include <string>

#include "base_enoding.h"
#include "base_event_emitter.h"
#include "base_stream.h"
#include "base_types.h"
#include "base_enoding.h"
#include "lib_http.h"
#include "lib_http_headers.h"
#include "lib_http_version.h"
#include "lib_net_socket_stream.h"
#include "base_stream.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				namespace impl {
					class HttpServerResponseImpl;
				}
				using HttpServerResponse = std::shared_ptr < impl::HttpServerResponseImpl >;
				HttpServerResponse create_http_server_response( std::weak_ptr<lib::net::impl::NetSocketStreamImpl> socket, base::EventEmitter emitter = base::create_event_emitter( ) );

				namespace impl {
					class HttpServerResponseImpl: public base::enable_shared< HttpServerResponseImpl >, public base::stream::StreamWritableEvents<HttpServerResponseImpl> {
						base::EventEmitter m_emitter;
						std::weak_ptr<lib::net::impl::NetSocketStreamImpl> m_socket;
						HttpVersion m_version;
						HttpHeaders m_headers;
						base::data_t m_body;
						bool m_status_sent;
						bool m_headers_sent;
						bool m_body_sent;
						
						HttpServerResponseImpl( std::weak_ptr<lib::net::impl::NetSocketStreamImpl> socket, base::EventEmitter emitter );
						
						bool on_socket_if_valid( std::function<void( lib::net::NetSocketStream )> action );

					public:
						friend HttpServerResponse lib::http::create_http_server_response( std::weak_ptr<lib::net::impl::NetSocketStreamImpl>, base::EventEmitter );

						HttpServerResponseImpl( HttpServerResponseImpl const & ) = delete;
						~HttpServerResponseImpl( ) = default;
						HttpServerResponseImpl& operator=(HttpServerResponseImpl const &) = delete;
	
						HttpServerResponseImpl( HttpServerResponseImpl&& other ) = delete;
						HttpServerResponseImpl& operator=(HttpServerResponseImpl && rhs) = delete;
							
						base::EventEmitter& emitter( );

						HttpServerResponseImpl& write( base::data_t const & data );
						HttpServerResponseImpl& write( boost::string_ref data, base::Encoding const & encoding = base::Encoding( ) );
						HttpServerResponseImpl& end( );
						HttpServerResponseImpl& end( base::data_t const & data );
						HttpServerResponseImpl& end( boost::string_ref data, base::Encoding const & encoding = base::Encoding( ) );
	
						void close( );
						void start( );

						HttpHeaders& headers( );
						HttpHeaders const & headers( ) const;
	
						HttpServerResponseImpl& send_status( uint16_t status_code = 200 );
						HttpServerResponseImpl& send_status( uint16_t status_code, boost::string_ref status_msg );
						HttpServerResponseImpl& send_headers( );
						HttpServerResponseImpl& send_body( );
						HttpServerResponseImpl& clear_body( );
						bool send( );
						HttpServerResponseImpl& reset( );
						bool is_open( );
						bool is_closed( ) const;
						bool can_write( ) const;
	
						HttpServerResponseImpl& add_header( std::string header_name, std::string header_value );
						
					};	// struct HttpServerResponseImpl						
				}	// namespace impl								

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
