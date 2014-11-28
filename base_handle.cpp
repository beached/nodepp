#include <boost/asio/io_service.hpp>

#include "base_handle.h"

namespace daw {
	namespace nodepp {
		namespace base {
			boost::asio::io_service Handle::s_io_service;

			boost::asio::io_service& Handle::get( ) {
				return s_io_service;
			}

			boost::asio::io_service const & Handle::get( ) const {
				return s_io_service;
			}
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
