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
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
