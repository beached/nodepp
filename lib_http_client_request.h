#pragma once

#include <string>

#include "base_enoding.h"

#include "base_event_emitter.h"
#include "lib_http_chunk.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				//////////////////////////////////////////////////////////////////////////
				// Summary:
				// Requires:	base::EventEmitter
				class HttpClientRequest: virtual public base::EventEmitter {
				public:
					HttpClientRequest( );

					std::vector<std::string> const & valid_events( ) const override;

					bool write( std::string chunk, base::Encoding const & encoding = base::Encoding{ } );
					bool write( HttpChunk const & chunk );

					void end( );
					void end( std::string data, base::Encoding const & encoding = base::Encoding{ } );
					void end( HttpChunk chunk );
					void abort( );

					template<typename Listener>
					HttpClientRequest& on( std::string event, Listener listener ) {
						add_listener( event, listener );
						return *this;
					}

					template<typename Listener>
					HttpClientRequest& once( std::string event, Listener listener ) {
						add_listener( event, listener, true );
						return *this;
					}
				};	// class HttpClientRequest

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
