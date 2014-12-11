#pragma once

#include <cstdint>
#include <string>
#include <utility>

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;

				class HttpVersion {
					std::pair<uint8_t, uint8_t> m_version;
					bool m_is_valid;
				public:
					uint8_t const & major( ) const;
					uint8_t & major( );
					uint8_t const & minor( ) const;
					uint8_t & minor( );
					HttpVersion( HttpVersion const & ) = default;
					HttpVersion& operator=(HttpVersion const &) = default;
					~HttpVersion( ) = default;

					HttpVersion( );
					HttpVersion( uint8_t const & Major, uint8_t const & Minor );
					HttpVersion( std::string const & version );
					HttpVersion& operator=(std::string const & version);
					std::string to_string( ) const;
					operator std::string( ) const;
					bool is_valid( ) const;
				};
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
