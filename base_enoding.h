#pragma once

#include <string>

namespace daw {
	namespace nodepp {
		namespace base {
			class Encoding {
				std::string m_encoding;
			public:
				explicit Encoding( std::string encoding = "utf8" );	
				Encoding( Encoding const & ) = default;
				Encoding& operator=(Encoding const &) = default;
				Encoding& operator=(std::string const & rhs);

				~Encoding( ) = default;

				std::string const & operator()( ) const;
				void set( std::string encoding );
				static bool is_valid_encoding( std::string encoding );

				operator std::string( ) const {
					return m_encoding;
				}

			};	// class Encoding
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw


