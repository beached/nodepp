#pragma once

#include <string>
#include "lib_event_emitter.h"
#include "lib_http_headers.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				enum class ServerResponseEvents { close, finish, newListener, removeListener };
				class ServerResponse: virtual public daw::nodepp::base::generic::EventEmitter < ServerResponseEvents > {
					// TODO: enable stream interface
					ServerResponse& write_continue( );

					ServerResponse& write_head( uint16_t status_code, std::string reason_phrase = "", Headers headers = Headers{ } );

					template<typename Listener>
					ServerResponse& set_timeout( size_t msecs, Listener listener ) {
						throw std::runtime_error( "Method Not Implemented" );
					}

					size_t & status_code( );
					size_t const & status_code( ) const;

					ServerResponse& set_header( std::string name, std::string value );
					
					bool headers_sent( ) const;

					std::string const & get_header( std::string const& name ) const;

					ServerResponse& remove_header( std::string const& name ) const;




				};	// class ServerResponse
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
