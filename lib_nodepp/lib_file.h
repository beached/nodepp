// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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

