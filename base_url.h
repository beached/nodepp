#pragma once

#include <cstdint>
#include <string>

namespace daw {
	namespace nodepp {
		namespace base {
				using namespace daw::nodepp;

				class Url {
					std::string m_url;
					std::pair<uint16_t, uint16_t> m_protocol;					
					std::pair<uint16_t, uint16_t> m_host_name;
					std::pair<uint16_t, uint16_t> m_auth_user;
					std::pair<uint16_t, uint16_t> m_auth_password;
					std::pair<uint16_t, uint16_t> m_port;
					std::pair<uint16_t, uint16_t> m_path;
					std::pair<uint16_t, uint16_t> m_query;
					std::pair<uint16_t, uint16_t> m_hash;
					mutable bool m_is_valid;
				public:
					Url( ) = default;
					Url( Url const & ) = default;
					Url& operator=(Url const &) = default;
					Url& operator=(std::string const & url);
					~Url( ) = default;

					Url( std::string const & uri );
					
					Url( std::string protocol, std::string host_port, std::string request_path );
					void from_string( std::string url );
					std::string to_string( ) const;
					bool is_valid( ) const;

					template<typename Iterator>
					Url( Iterator first, Iterator last ): m_url( firt, last ) { }

				};
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
