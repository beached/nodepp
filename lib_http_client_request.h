#pragma once

#include <string>

#include "base_enoding.h"

#include "base_event_emitter.h"
#include "lib_http_chunk.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				
				class Request { };
				class IncomingMessage { };
				
				class ClientRequest: public daw::nodepp::base::EventEmitter {
				protected:
					virtual bool event_is_valid( std::string const & event ) const override;
				public:
					ClientRequest( );
					bool write( std::string chunk, daw::nodepp::base::Encoding const & encoding = daw::nodepp::base::Encoding{ } );
					bool write( Chunk const & chunk );

					void end( );
					void end( std::string const & data, daw::nodepp::base::Encoding const & encoding = daw::nodepp::base::Encoding{ } );
					void end( Chunk const & chunk );
					void abort( );

					template<typename Listener>
					ClientRequest& on( std::string event, Listener& listener ) {
						add_listener( event, listener );
						return *this;
					}

					template<typename Listener>
					ClientRequest& once( std::string event, Listener& listener ) {
						add_listener( event, listener, true );
						return *this;
					}
				};

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
