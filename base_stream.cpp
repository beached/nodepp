#include <string>

#include "base_event_emitter.h"
#include "base_stream.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace base {
			namespace stream {
				using namespace daw::nodepp;
				std::vector<std::string> const & StreamReadable::valid_events( ) const {
					static auto const result = [&]( ) {
						auto local = std::vector < std::string > { "readable", "data", "end", "close", "error" };
						return base::impl::append_vector( local, EventEmitter::valid_events( ) );
					}();
					return result;
				}

				std::vector<std::string> const & StreamWritable::valid_events( ) const {
					static auto const result = [&]( ) {
						auto local = std::vector < std::string > { "drain", "finish", "pipe", "unpipe", "error" };
						return base::impl::append_vector( local, EventEmitter::valid_events( ) );
					}();
					return result;
				}

				std::vector<std::string> const & Stream::valid_events( ) const {
					static auto const result = [&]( ) {
						auto local = StreamReadable::valid_events( );
						return base::::append_vector( local, StreamWritable::valid_events( ) );
					}();
					return result;
				}
			}	// namespace stream
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
