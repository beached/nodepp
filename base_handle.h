#pragma once

#include <boost/asio.hpp>
#include <memory>


namespace daw {
	namespace nodepp {
		namespace base {
			class Handle {
				static boost::asio::io_service s_io_service;
			public:
				Handle( ) = default;
				Handle( Handle const & ) = default;
				Handle& operator=(Handle const &) = default;
				~Handle( ) = default;
	
				boost::asio::io_service& get( );
				boost::asio::io_service const & get( ) const;

			};	// class Handle
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
