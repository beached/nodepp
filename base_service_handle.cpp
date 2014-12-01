#include <boost/asio/io_service.hpp>

#include "base_service_handle.h"

namespace daw {
	namespace nodepp {
		namespace base {
			boost::asio::io_service& ServiceHandle::get( ) {
				static boost::asio::io_service result;
				return result;
			}
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
