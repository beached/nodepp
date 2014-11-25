#pragma once

//#include <boost/asio.hpp>

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				class Handle {
				protected:	
					//boost::asio::io_service m_io_service;
				public:
					Handle( ) = default;
					Handle( Handle const & ) = default;
					Handle& operator=( Handle const & ) = default;
					virtual ~Handle( ) = default;
					//m_handle;
				};	// class Handle
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
