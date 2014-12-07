#pragma once

#include <string>
#include "base_event_emitter.h"
#include "lib_http_headers.h"
#include "base_enoding.h"
#include "base_stream.h"
#include "lib_http_chunk.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				//////////////////////////////////////////////////////////////////////////
				// Summary:		
				// Requires:	base::EventEmitter, lib::http::HttpHeader,
				//				lib::http::HttpHeaders, lib::http::HttpChunk,
				//				base::Encodingn
				class HttpServerResponse: public base::stream::StreamWritable {
					HttpServerResponse( );
					friend class HttpServer;
				public:
					

					HttpServerResponse& write_continue( );
					HttpServerResponse& write_head( uint16_t status_code, std::string reason_phrase = "", HttpHeaders headers = HttpHeaders{ } );
					
					template<typename Listener>
					HttpServerResponse& set_timeout( size_t msecs, Listener listener ) {
						throw std::runtime_error( "Method not implemented" );
					}
					
					uint16_t& status_code( );
					uint16_t const & status_code( ) const;

					void set_header( std::string name, std::string value );
					void set_header( HttpHeaders headers );

					bool headers_sent( ) const;

					bool& send_date( );
					bool const& send_date( ) const;

					HttpHeader const & get_header( std::string name ) const;
					HttpServerResponse& remove_header( std::string name );

					bool write_chunk( std::string const & chunk, base::Encoding const & encoding = base::Encoding{ } );
					bool write_chunk( base::data_t const & chunk, base::Encoding const & encoding = base::Encoding{ } );

					bool add_trailers( HttpHeaders headers );

					// StreamWriteable overrides
					virtual std::vector<std::string> const & valid_events( ) const override;
					virtual HttpServerResponse& write( base::data_t const & chunk ) override;
					virtual HttpServerResponse& write( std::string const & chunk, base::Encoding const & encoding = base::Encoding( ) ) override;
					virtual HttpServerResponse& end( ) override;
					virtual HttpServerResponse& end( base::data_t const & chunk ) override;
					virtual HttpServerResponse& end( std::string const & chunk, base::Encoding const & encoding = base::Encoding( ) ) override;
				};	// class ServerResponse			


			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
