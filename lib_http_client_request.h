#pragma once

#include <string>

#include "base_enoding.h"

#include "base_event_emitter.h"
#include "lib_http_chunk.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				
				class HttpClientRequest: public daw::nodepp::base::EventEmitter {
				protected:
					virtual bool event_is_valid( std::string const & event ) const override;
				public:
					HttpClientRequest( );
					bool write( std::string chunk, daw::nodepp::base::Encoding const & encoding = daw::nodepp::base::Encoding{ } );
					bool write( HttpChunk const & chunk );

					void end( );
					void end( std::string const & data, daw::nodepp::base::Encoding const & encoding = daw::nodepp::base::Encoding{ } );
					void end( HttpChunk const & chunk );
					void abort( );

					template<typename Listener>
					HttpClientRequest& on( std::string event, Listener& listener ) {
						add_listener( event, listener );
						return *this;
					}

					template<typename Listener>
					HttpClientRequest& once( std::string event, Listener& listener ) {
						add_listener( event, listener, true );
						return *this;
					}
				};

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
