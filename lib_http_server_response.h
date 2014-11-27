#pragma once

#include <string>
#include "base_event_emitter.h"
#include "lib_http_headers.h"
#include "base_enoding.h"
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
				class HttpServerResponse: virtual public base::EventEmitter {
					HttpServerResponse( );
					friend class HttpServer;
				public:
					virtual std::vector<std::string> const & valid_events( ) const override;

					HttpServerResponse& write_continue( );
					HttpServerResponse& write_head( uint16_t status_code, std::string reason_phrase = "", Headers headers = Headers{ } );
					
					template<typename Listener>
					HttpServerResponse& set_timeout( size_t msecs, Listener listener ) {
						throw std::runtime_error( __func__"Method not implemented" );
					}
					
					uint16_t& status_code( );
					uint16_t const & status_code( ) const;

					void set_header( std::string name, std::string value );
					void set_header( Headers headers );

					bool headers_sent( ) const;

					bool& send_date( );
					bool const& send_date( ) const;

					HttpHeader const & get_header( std::string name ) const;
					HttpServerResponse& remove_header( std::string name );

					bool write_chunk( std::string const & chunk, base::Encoding encoding = base::Encoding{ } );
					bool write_chunk( HttpChunk const & chunk );

					bool add_trailers( Headers headers );

					void end( );
					void end( HttpChunk chunk );
					void end( std::string const & chunk, base::Encoding encoding = base::Encoding{ } );

					template<typename Listener>
					HttpServerResponse& on( std::string event, Listener& listener ) {
						add_listener( event, listener );
						return *this;
					}

					template<typename Listener>
					HttpServerResponse& once( std::string event, Listener& listener ) {
						add_listener( event, listener, true );
						return *this;
					}
				};	// class ServerResponse			


			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
