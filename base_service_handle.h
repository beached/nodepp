#pragma once

#include <boost/asio.hpp>
#include <memory>


namespace daw {
	namespace nodepp {
		namespace base {
			class ServiceHandle {
			public:
				static boost::asio::io_service& get( );
				static void run( );
			};	// class ServiceHandle

			enum class StartServiceMode { Single, OnePerCore };

			void start_service( StartServiceMode mode );
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
