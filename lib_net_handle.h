#pragma once

#include <boost/asio.hpp>
#include <memory>


namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				class NetHandle {
				protected:	
					std::shared_ptr<boost::asio::io_service> m_io_service;
				public:
					NetHandle( ) = default;
					NetHandle( NetHandle const & ) = default;
					NetHandle& operator=( NetHandle const & ) = default;
					virtual ~NetHandle( ) = default;
					//m_handle;
				};	// class Handle
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
