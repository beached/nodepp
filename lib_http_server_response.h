#pragma once

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

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				//////////////////////////////////////////////////////////////////////////
				// Summary:	Contains the data needed to respond to a client request				
				class HttpServerResponse {	// TODO inherit from StreamWriterHt
					HttpVersion m_version;
					HttpHeaders m_headers;
					base::data_t m_body;
					bool m_status_sent;
					bool m_headers_sent;
					bool m_body_sent;
					std::shared_ptr<lib::net::NetSocketStream> m_socket_ptr;
				public:
					HttpServerResponse( std::shared_ptr<lib::net::NetSocketStream> socket_ptr );
					HttpServerResponse( HttpServerResponse const & ) = default;
					~HttpServerResponse( ) = default;
					HttpServerResponse& operator=(HttpServerResponse const &) = default;
					HttpServerResponse( HttpServerResponse&& other );
					HttpServerResponse& operator=(HttpServerResponse && rhs);
					HttpServerResponse& write( base::data_t data );
					HttpServerResponse& write( std::string data, base::Encoding encoding = base::Encoding( ) );
					void end( );
					void end( base::data_t data );
					void end( std::string data, base::Encoding encoding = base::Encoding( ) );

					void close( );

					HttpHeaders& headers( );
					HttpHeaders const & headers( ) const;

					void send_status( uint16_t status_code = 200 );
					void send_headers( );
					void send_body( );
					void clear_body( );
					void send( );
					void reset( );
					bool is_open( );
					bool is_closed( ) const;
					bool can_write( ) const;

					HttpServerResponse& add_header( std::string header_name, std::string header_value );
				};	// struct ServerResponse			
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
