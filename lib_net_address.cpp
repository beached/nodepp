#include <stdexcept>
#include "lib_net_address.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				NetAddress::NetAddress( ) : m_address( "0.0.0.0" ) { }

				NetAddress::NetAddress( std::string address ) : m_address( std::move( address ) ) {
					if( !is_valid( m_address ) ) {
						throw std::runtime_error( "Invalid address" );
					}
				}
				NetAddress::NetAddress( NetAddress && other):m_address( std::move( other.m_address ) ) { }

				NetAddress& NetAddress::operator=(NetAddress && rhs) {
					if( this != &rhs ) {
						m_address = std::move( rhs.m_address );
					}
					return *this;
				}

				boost::string_ref NetAddress::operator()( ) const {
					return m_address;
				}

				bool NetAddress::is_valid( std::string address ) {
					return true;	// TODO complete
				}
			}	// namespace lib
		}	// namespace net
	}	// namespace nodepp
}	// namespace daw
