#include <stdexcept>
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

			Encoding::Encoding( ) : m_encoding( "utf8" ) { }

			Encoding::Encoding( std::string encoding ) : m_encoding( encoding ) { }

			Encoding& Encoding::operator = (boost::string_ref rhs) {
				if( !is_valid_encoding( rhs ) ) {
					throw std::runtime_error( "Encoding is not valid" );
				}
				m_encoding = rhs.to_string( );
				return *this;
			}

			Encoding::Encoding( Encoding && other ): m_encoding( std::move( other.m_encoding ) ) { }
			Encoding& Encoding::operator = (Encoding && rhs) {
				if( this != &rhs ) {
					m_encoding = std::move( rhs.m_encoding );
				}
				return *this;
			}


			boost::string_ref Encoding::operator()( ) const { return m_encoding; }

			void Encoding::set( std::string encoding ) {
				if( !is_valid_encoding( encoding ) ) {
					throw std::runtime_error( "Encoding is not valid" );
				}
				m_encoding = encoding;
			}

			bool Encoding::is_valid_encoding( boost::string_ref ) {
				// TODO: validate the encoding
				return true;
			}

		
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw


