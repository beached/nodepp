#pragma once

#include <mutex>

#include "base_event_emitter.h"
#include "base_stream.h"
#include "base_write_buffer.h"

namespace daw {
	namespace nodepp {
		namespace lib {	
			namespace file {
				using namespace daw::nodepp;

				//////////////////////////////////////////////////////////////////////////
				/// Summary:	Reads in contents of file and appends it to buffer
				base::OptionalError read_file( boost::string_ref path, base::data_t & buffer, bool append_buffer = true );
				void read_file_async( boost::string_ref path, std::function<void( base::OptionalError error, std::shared_ptr<base::data_t> data )> listener, std::shared_ptr<base::data_t> buffer = nullptr, bool append_buffer = true );
				std::streampos file_size( boost::string_ref path );
			}	// namespace file
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
