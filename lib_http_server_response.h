#pragma once

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
				struct HttpServerResponse {	// TODO inherit from StreamWriter
					HttpVersion version;
					HttpHeaders headers;
					http_status_code_t status;
					base::data_t body;
					bool is_chunk;
					std::shared_ptr<HttpServerResponse> next;



					HttpServerResponse( );

					HttpServerResponse& write( base::data_t data );
					HttpServerResponse& write( std::string data, base::Encoding encoding = base::Encoding( ) );
					
					template<typename... Args>
					HttpServerResponse& create_chunk( Args&&... args ) {
						next = std::make_shared<HttpServerResponse>( std::forward<Args>( args )... );
						return *next;
					}

					void clear_body( );
				};	// struct ServerResponse			
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
