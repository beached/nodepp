
#include <string>
#include <vector>
#include "base_enoding.h"

namespace daw {
	namespace nodepp {
		namespace base {
			std::vector<std::string> const & Encoding::valid_enodings( ) {
				static const std::vector<std::string> result = { "ascii", "utf8", "utf16le", "ucs2", "hex" };
				return result;
			}

			Encoding::Encoding( std::string encoding ) { }

			Encoding& Encoding::operator = (std::string const & rhs) {
				if( !is_valid_encoding( rhs ) ) {
					throw std::runtime_error( "Encoding is not valid" );
				}
				m_encoding = rhs;
				return *this;
			}

			std::string const & Encoding::operator()( ) const { return m_encoding; }

			void Encoding::set( std::string encoding ) {
				if( !is_valid_encoding( encoding ) ) {
					throw std::runtime_error( "Encoding is not valid" );
				}
				m_encoding = encoding;
			}

			bool Encoding::is_valid_encoding( std::string encoding ) {
				// TODO: validate the encoding
				return true;
			}

		
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw


