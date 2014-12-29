#pragma once

#include <boost/asio.hpp>
#include <functional>
#include <memory>

#include "base_error.h"

namespace daw {
	namespace nodepp {
		namespace base {
			struct ServiceHandle {
				static boost::asio::io_service& get( );
				static void run( );
				static void work( );
			};	// struct ServiceHandle

			enum class StartServiceMode { Single, OnePerCore };

			void start_service( StartServiceMode mode );
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
