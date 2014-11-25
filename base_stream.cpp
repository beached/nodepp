#include <string>

#include "base_event_emitter.h"
#include "base_stream.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace base {
			bool ReadableStream::event_is_valid( std::string const & event ) const {
				static std::vector<std::string> const valid_events = { "readable", "data", "end", "close", "error" };
				return daw::algorithm::find( valid_events, event ) != valid_events.end( ) || daw::nodepp::base::EventEmitter::event_is_valid( event );
			}

		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
