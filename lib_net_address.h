#pragma once

#include <string>

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				class NetAddress {
					std::string m_address;
				public:
					NetAddress( );
					~NetAddress( ) = default;
					NetAddress( NetAddress const & ) = default;
					NetAddress( NetAddress && other );
					explicit NetAddress( std::string address );

					NetAddress& operator=(NetAddress const &) = default;
					NetAddress& operator=(NetAddress && rhs);


					std::string const & operator()( ) const;

					static bool is_valid( std::string address );
				};	// class NetAddress;
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
