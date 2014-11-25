#pragma once

#include <string>

#include "base_event_emitter.h"
#include "base_types.h"

namespace daw {
	namespace nodepp {
		namespace base {
			class ReadableStream: public daw::nodepp::base::EventEmitter {
			protected:
				virtual bool event_is_valid( std::string const & event ) const;
			public:
				data_t read( );
				data_t read( size_t bytes );


				template<typename Listener>
				ReadableStream& on( std::string event, Listener& listener ) {
					add_listener( event, listener );
					return *this;
				}

				template<typename Listener>
				ReadableStream& once( std::string event, Listener& listener ) {
					add_listener( event, listener, true );
					return *this;
				}
			};
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
