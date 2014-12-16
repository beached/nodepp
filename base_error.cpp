#include <boost/lexical_cast.hpp>
#include <boost/system/error_code.hpp>
#include <stdexcept>
#include <string>
#include "base_error.h"

namespace daw {
	namespace nodepp {
		namespace base {
			Error::Error( std::string description ) : m_keyvalues{ }, m_frozen{ false }, m_child{ }, m_exception{ } {
				m_keyvalues.emplace( "description", description );
			}

			Error::Error( boost::system::error_code const & err ) : m_keyvalues{ }, m_frozen{ false }, m_child{ }, m_exception{ } {
				m_keyvalues.emplace( "description", err.message( ) );
				m_keyvalues.emplace( "category", boost::lexical_cast<std::string>(err.category( ).name( )) );
				m_keyvalues.emplace( "error_code", boost::lexical_cast<std::string>(err.value( )) );
 			}

			Error::Error( std::string description, std::exception_ptr ex_ptr ) : m_keyvalues( ), m_frozen( false ), m_child( ), m_exception( std::move( ex_ptr ) ) { 
				m_keyvalues.emplace( "description", description );
			}

			Error::Error( Error && other ) : m_keyvalues( std::move( other.m_keyvalues ) ), m_frozen{ std::move( other.m_frozen ) }, m_child{ std::move( other.m_child ) }, m_exception{ std::move( other.m_exception ) } { }

			Error& Error::operator=(Error && rhs) {
				if( this != &rhs ) {
					m_keyvalues = std::move( rhs.m_keyvalues );
					m_frozen = std::move( rhs.m_frozen );
					m_child = std::move( rhs.m_child );
					m_exception = std::move( rhs.m_exception );
				}
				return *this;
			}

			Error& Error::add( std::string const & name, std::string value ) {
				if( m_frozen ) {
					throw std::runtime_error( "Attempt to change a frozen error." );
				}
				m_keyvalues[name] = std::move( value );
				return *this;
			}


			std::string const & Error::get( std::string const & name ) const {
				return m_keyvalues.at( name );
			}

			std::string & Error::get( std::string const & name ) {
				return m_keyvalues[name];
			}

			void Error::freeze( ) {
				m_frozen = true;
			}

			Error & Error::child( ) const {
				return *m_child.get( );
			}

			bool Error::has_child( ) const {
				if( m_child ) {
					return true;
				}
				return false;
			}

			bool Error::has_exception( ) const {
				if( has_child() && child( ).has_exception( ) ) {
					return true;
				}
				return static_cast<bool>(m_exception);
			}

			void Error::throw_exception( ) {
				if( has_child( ) && child( ).has_exception( ) ) {
					child( ).throw_exception( );
				}
				if( has_exception( ) ) {
					auto current_exception = std::move( m_exception );
					m_exception = nullptr;
					std::rethrow_exception( current_exception );
				}
			}

			Error& Error::clear_child( ) {
				m_child.reset( );
				return *this;
			}

			Error& Error::child( Error child ) {
				m_child = std::make_shared<Error>( std::move( child ) );
				m_child->freeze( );
				return *this;
			}

			std::string Error::to_string( std::string prefix ) const {
				std::stringstream ss;
				ss << "Description: " << m_keyvalues.at( "description" ) << "\n";
				for( auto const & row : m_keyvalues ) {
					if( row.first.compare( "description" ) != 0 ) {
						ss << prefix << "'" << row.first << "',	'" << row.second << "'\n";
					}
				}
				if( has_child( ) ) {
					ss << child( ).to_string( prefix + "	" );
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
