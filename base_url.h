#pragma once

#include <cstdint>
#include <string>

namespace daw {
	namespace nodepp {
		namespace base {
				using namespace daw::nodepp;

				class Url {
					std::string m_protocol;
					std::string m_host_name;
					std::string m_auth_user;
					std::string m_auth_password;
					std::string m_port;
					std::string m_path;
					std::string m_query;
					std::string m_hash;
				public:
					Url( ) = default;
					Url( Url const & ) = default;
					Url& operator=(Url const &) = default;
					Url& operator=(std::string const & url);
					~Url( ) = default;

					Url( std::string const & uri );

					Url( std::string protocol, std::string host_port, std::string request_path );
					void from_string( std::string const & url );
					std::string to_string( ) const;
				};
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
