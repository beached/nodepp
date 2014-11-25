#include <stdexcept>
#include <string>
#include "base_error.h"

namespace daw {
	namespace nodepp {
		namespace base {
			Error::Error( std::string description ) : m_keyvalues{ }, m_frozen{ false } { 
				m_keyvalues.emplace( "description", description );
			}

			Error::Error( Error && other ) : m_keyvalues( std::move( other.m_keyvalues ) ), m_frozen{ std::move( other.m_frozen ) } { }

			Error& Error::operator=(Error && rhs) {
				if( this != &rhs ) {
					m_keyvalues = std::move( rhs.m_keyvalues );
					m_frozen = std::move( rhs.m_frozen );
				}
				return *this;
			}

			Error& Error::add( std::string name, std::string value ) {
				if( m_frozen ) {
					throw std::runtime_error( "Attempt to change a frozen error." );
				}
				m_keyvalues.emplace( std::move( name ), std::move( value ) );
				return *this;
			}

			void Error::freeze( ) {
				m_frozen = true;
			}
	}	// namespace base
}	// namespace nodepp
}	// namespace daw
