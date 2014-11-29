#include <boost/asio/io_service.hpp>

#include "base_handle.h"

namespace daw {
	namespace nodepp {
		namespace base {
			boost::asio::io_service& Handle::get( ) {
				static boost::asio::io_service result;
				return result;
			}
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
