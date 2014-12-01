#include <boost/asio/ip/tcp.hpp>
#include <memory>
#include "base_service_handle.h"
#include "lib_net_socket_handle.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				SocketHandle::SocketHandle( ) : m_socket( std::make_shared<boost::asio::ip::tcp::socket>( base::ServiceHandle::get( ) )	) { }
				SocketHandle::SocketHandle( SocketHandle && other ): m_socket( std::move( other.m_socket ) ) { }

				SocketHandle& SocketHandle::operator=(SocketHandle && rhs) {
					if( this != &rhs ) {
						m_socket = std::move( rhs.m_socket );
					}
					return *this;
				}

				boost::asio::ip::tcp::socket& SocketHandle::get( ) { return *m_socket; }
				boost::asio::ip::tcp::socket const & SocketHandle::get( ) const { return *m_socket; }

				boost::asio::ip::tcp::socket& SocketHandle::operator*() {
					return *m_socket;
				}

				boost::asio::ip::tcp::socket const & SocketHandle::operator*() const {
					return *m_socket;
				}

				boost::asio::ip::tcp::socket* SocketHandle::operator->() { 
					return m_socket.get( );
				}

				boost::asio::ip::tcp::socket const * SocketHandle::operator->() const {
					return m_socket.get( );
				}

			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
