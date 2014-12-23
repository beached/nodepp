#pragma once
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
					class HttpServerResponseImpl: public base::enabled_shared< HttpServerResponseImpl >, public base::stream::StreamWritableEvents<HttpServerResponseImpl> {
						base::EventEmitter m_emitter;
						std::weak_ptr<lib::net::impl::NetSocketStreamImpl> m_socket;
						HttpVersion m_version;
						HttpHeaders m_headers;
						base::data_t m_body;
						bool m_status_sent;
						bool m_headers_sent;
						bool m_body_sent;
						
						HttpServerResponseImpl( std::weak_ptr<lib::net::impl::NetSocketStreamImpl> socket, base::EventEmitter emitter );
					public:
						friend HttpServerResponse lib::http::create_http_server_response( std::weak_ptr<lib::net::impl::NetSocketStreamImpl>, base::EventEmitter );

						HttpServerResponseImpl( HttpServerResponseImpl const & ) = delete;
						~HttpServerResponseImpl( ) = default;
						HttpServerResponseImpl& operator=(HttpServerResponseImpl const &) = delete;
	
						HttpServerResponseImpl( HttpServerResponseImpl&& other ) = delete;
						HttpServerResponseImpl& operator=(HttpServerResponseImpl && rhs) = delete;
	
//						std::shared_ptr<HttpServerResponseImpl> get_ptr( );
						
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
