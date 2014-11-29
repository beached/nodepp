#pragma once

#include <boost/asio.hpp>
#include <memory>


namespace daw {
	namespace nodepp {
		namespace base {
			class Handle {
			public:
				static boost::asio::io_service& get( );
			};	// class Handle
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
