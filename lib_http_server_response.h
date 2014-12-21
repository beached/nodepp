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
				HttpServerResponse create_http_server_response( lib::net::SharedNetSocketStream, base::SharedEventEmitter );

				namespace impl {
					class HttpServerResponseImpl: public std::enable_shared_from_this < HttpServerResponseImpl >, public base::stream::StreamWritableEvents<HttpServerResponseImpl> {
						base::SharedEventEmitter m_emitter;
						lib::net::SharedNetSocketStream m_socket;
						HttpVersion m_version;
						HttpHeaders m_headers;
						base::data_t m_body;
						bool m_status_sent;
						bool m_headers_sent;
						bool m_body_sent;
						
						HttpServerResponseImpl( lib::net::SharedNetSocketStream socket, base::SharedEventEmitter emitter );
					public:
						friend HttpServerResponse lib::http::create_http_server_response( lib::net::SharedNetSocketStream, base::SharedEventEmitter );

						HttpServerResponseImpl( HttpServerResponseImpl const & ) = delete;
						~HttpServerResponseImpl( ) = default;
						HttpServerResponseImpl& operator=(HttpServerResponseImpl const &) = delete;;
	
						HttpServerResponseImpl( HttpServerResponseImpl&& other ) = delete;
						HttpServerResponseImpl& operator=(HttpServerResponseImpl && rhs) = delete;
	
						std::shared_ptr<HttpServerResponseImpl> get_ptr( );
	
						void write( base::data_t const & data );
						void write( boost::string_ref data, base::Encoding const & encoding = base::Encoding( ) );
						void end( );
						void end( base::data_t const & data );
						void end( boost::string_ref data, base::Encoding const & encoding = base::Encoding( ) );
	
						void close( );
	
						HttpHeaders& headers( );
						HttpHeaders const & headers( ) const;
	
						void send_status( uint16_t status_code = 200 );
						void send_headers( );
						void send_body( );
						void clear_body( );
						bool send( );
						void reset( );
						bool is_open( );
						bool is_closed( ) const;
						bool can_write( ) const;
	
						void add_header( std::string header_name, std::string header_value );
						
					};	// struct HttpServerResponseImpl						
				}	// namespace impl				

				HttpServerResponse create_http_server_response( lib::net::SharedNetSocketStream socket, base::SharedEventEmitter emitter = base::create_shared_event_emitter( ) ) {
					return HttpServerResponse( new impl::HttpServerResponseImpl( std::move( socket ), std::move( emitter ) ) );
				}

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
