#include <boost/asio/io_service.hpp>

#include "base_handle.h"

namespace daw {
	namespace nodepp {
		namespace base {
			boost::asio::io_service& Handle::handle( ) {
				static boost::asio::io_service result;
				return result;
			}

			boost::asio::io_service& Handle::get( ) {
				return handle( );
			}
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
