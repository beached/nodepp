first
first
first
#include <boost/lexical_cast.hpp>
#include <boost/system/error_code.hpp>
#include <stdexcept>
#include <string>
#include "base_error.h"
#include "make_unique.h"

namespace daw {
	namespace nodepp {
		namespace base {
			Error::Error( std::string description ) : m_keyvalues{ }, m_frozen{ false } { 
				m_keyvalues.emplace( "description", description );
			}

			Error::Error( boost::system::error_code const & err ) : m_keyvalues{ }, m_frozen{ false } {
				m_keyvalues.emplace( "description", err.message( ) );
				m_keyvalues.emplace( "category", boost::lexical_cast<std::string>(err.category( ).name( )) );
				m_keyvalues.emplace( "error_code", boost::lexical_cast<std::string>(err.value( )) );
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

			Error const & Error::get_child( ) const {
				return *m_child.get( );
			}

			bool Error::has_child( ) const {
				if( m_child ) {
					return true;
				}
				return false;
			}

			Error& Error::clear_child( ) {
				m_child.reset( nullptr );
				return *this;
			}

			Error& Error::set_child( Error const & child ) {
				m_child = daw::make_unique<Error>( child );
				m_child->freeze( );
				return *this;
			}

			std::string Error::to_string( std::string prefix = "" ) const {
				std::stringstream ss;
				ss << "Description: " << m_keyvalues["description"] << "\n";
				for( auto & const row : m_keyvalues ) {
					if( row.first.compare( "description" ) != 0 ) {
						ss << prefix << "'" << row.first << "',	'" << row.second << "'\n";
					}
				}
				if( m_child ) {
					ss << m_child->to_string( prefix + "	" );
				}
				ss << "\n";
				return ss.str( );
			}

			std::ostream& operator<<(std::ostream& os, Error const & error) {
				os << error.to_string( );
				return os;
			}
	}	// namespace base
}	// namespace nodepp
}	// namespace daw
