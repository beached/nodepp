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

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				//////////////////////////////////////////////////////////////////////////
				// Summary:	Contains the data needed to respond to a client request				
				class HttpServerResponse {	// TODO inherit from StreamWriter
					HttpVersion m_version;
					HttpHeaders m_headers;
					http_status_code_t m_status;
					base::data_t m_body;
				public:
					HttpServerResponse( uint16_t status_code = 200 );

					HttpServerResponse& write( base::data_t data );
					HttpServerResponse& write( std::string data, base::Encoding encoding = base::Encoding( ) );
					
					template<typename... Args>
					HttpServerResponse& create_chunk( Args&&... args ) {
						auto next = std::make_shared<HttpServerResponse>( std::forward<Args>( args )... );
						return *next;
					}
					void send_status( );
					void send_headers( );
					void send_body( );
					void clear_body( );
				};	// struct ServerResponse			
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
