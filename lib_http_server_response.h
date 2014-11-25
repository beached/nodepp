#pragma once

#include <string>
#include "base_event_emitter.h"
#include "lib_http_headers.h"
#include "lib_http_chunk.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {				
				class ServerResponse: public daw::nodepp::base::EventEmitter {
					ServerResponse( );
					friend class Server;
					virtual bool event_is_valid( std::string const & event ) const override;
				public:

					ServerResponse& write_continue( );
					ServerResponse& write_head( uint16_t status_code, std::string reason_phrase = "", Headers headers = Headers{ } );
					
					template<typename Listener>
					ServerResponse& set_timeout( size_t msecs, Listener listener ) {
						throw std::runtime_error( "Method Not Implemented" );
					}
					
					uint16_t& status_code( );
					uint16_t const & status_code( ) const;

					void set_header( std::string name, std::string value );
					void set_header( Headers headers );

					bool headers_sent( ) const;

					bool& send_date( );
					bool const& send_date( ) const;

					Header const & get_header( std::string name ) const;
					ServerResponse& remove_header( std::string name );

					bool write_chunk( Chunk chunk, daw::nodepp::base::Encoding encoding = daw::nodepp::base::Encoding{ } );
					bool add_trailers( Headers headers );

					void end( );
					void end( Chunk chunk, daw::nodepp::base::Encoding encoding = daw::nodepp::base::Encoding{ } );

					template<typename Listener>
					ServerResponse& on( std::string event, Listener& listener ) {
						add_listener( event, listener );
						return *this;
					}

					template<typename Listener>
					ServerResponse& once( std::string event, Listener& listener ) {
						add_listener( event, listener, true );
						return *this;
					}
				};	// class ServerResponse			


			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
