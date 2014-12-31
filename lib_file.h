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

				std::streampos file_size( boost::string_ref path );

				//////////////////////////////////////////////////////////////////////////
				/// Summary:	Reads in contents of file and appends it to buffer
				base::OptionalError read_file( boost::string_ref path, base::data_t & buffer, bool append_buffer = true );

				void read_file_async( boost::string_ref path, std::function<void( base::OptionalError error, std::shared_ptr<base::data_t> data )> callback, std::shared_ptr<base::data_t> buffer = nullptr, bool append_buffer = true );

				enum class FileWriteMode { OverwriteOrCreate, AppendOrCreate, MustBeNew };

				base::OptionalError write_file( boost::string_ref path, base::data_t const & buffer, FileWriteMode mode = FileWriteMode::MustBeNew, size_t bytes_to_write = 0 );
				int64_t write_file_async( boost::string_ref path, base::data_t const & buffer, std::function<void( base::OptionalError error )> callback, FileWriteMode mode = FileWriteMode::MustBeNew, size_t bytes_to_write = 0 );


			}	// namespace file
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
