#pragma once

#include <string>
#include <vector>

#include "base_event_emitter.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				class Chunk {
				public:
					Chunk( ) = default;
					Chunk( Chunk const & ) = default;
					Chunk& operator=( Chunk const & ) = default;
					virtual ~Chunk( ) = default;
					//m_Chunk;
				};	// class chunk
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
