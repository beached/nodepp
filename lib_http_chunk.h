#pragma once

#include <string>
#include <vector>

#include "base_event_emitter.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				class HttpChunk {
				public:
					HttpChunk( ) = default;
					HttpChunk( HttpChunk const & ) = default;
					HttpChunk& operator=( HttpChunk const & ) = default;
					virtual ~HttpChunk( ) = default;
					//m_Chunk;
				};	// class chunk
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
