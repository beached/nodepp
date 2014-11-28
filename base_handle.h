#pragma once

#include <boost/asio.hpp>
#include <memory>


namespace daw {
	namespace nodepp {
		namespace base {
			class Handle {
				boost::asio::io_service& handle( );
			public:
				Handle( ) = default;
				Handle( Handle const & ) = default;
				Handle& operator=(Handle const &) = default;
				~Handle( ) = default;
	
				boost::asio::io_service& get( );

			};	// class Handle
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
