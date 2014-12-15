#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <memory>

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				class SocketHandle {
					std::shared_ptr<boost::asio::ip::tcp::socket> m_socket;
				public:
					SocketHandle( );
					explicit SocketHandle( boost::asio::io_service & io_service );
					SocketHandle( SocketHandle const & ) = default;
					SocketHandle& operator=(SocketHandle const &) = default;
					
					~SocketHandle( );
					boost::asio::ip::tcp::socket& get( );
					boost::asio::ip::tcp::socket const & get( ) const;

					boost::asio::ip::tcp::socket& operator*();
					boost::asio::ip::tcp::socket const & operator*() const;

					boost::asio::ip::tcp::socket* operator->();
					boost::asio::ip::tcp::socket const * operator->() const;

					explicit operator bool( ) const;
				};	// class SocketHandle
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
