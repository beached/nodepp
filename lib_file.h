#pragma once

#include "base_event_emitter.h"
#include "base_stream.h"


namespace daw {
	namespace nodepp {
		namespace lib {	
			namespace impl {
				class LibFileImpl;
			}	// namespace impl
			
			using LibFile = std::shared_ptr < impl::LibFileImpl > ;

			LibFile create_file( base::EventEmitter emitter = base::create_event_emitter( ) );

			namespace impl {
				using namespace daw::nodepp;

				class LibFileImpl: public base::StandardEvents<LibFileImpl>, base::stream::StreamReadableEvents<LibFileImpl>, base::stream::StreamWritableEvents < LibFileImpl > {
					base::EventEmitter m_emitter;
				public:
					friend LibFile create_file( base::EventEmitter );

					base::EventEmitter& emitter( );
				
				};	// class LibFileImpl

			}	// namespace impl
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
