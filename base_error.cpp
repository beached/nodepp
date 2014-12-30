#include <boost/lexical_cast.hpp>
#include <boost/system/error_code.hpp>
#include <stdexcept>
#include <string>
#include "base_error.h"

namespace daw {
	namespace nodepp {
		namespace base {
			Error::Error( boost::string_ref description ) : m_keyvalues { }, m_frozen { false }, m_child { }, m_exception { } {
				m_keyvalues.emplace( "description", description.to_string() );
			}

			Error::Error( boost::system::error_code const & err ) : m_keyvalues{ }, m_frozen{ false }, m_child{ }, m_exception{ } {
				m_keyvalues.emplace( "description", err.message( ) );
				m_keyvalues.emplace( "category", std::string(err.category( ).name( )) );
				m_keyvalues.emplace( "error_code", std::to_string(err.value( )) );
 			}

			Error::Error( boost::string_ref description, std::exception_ptr ex_ptr ) : m_keyvalues( ), m_frozen( false ), m_child( ), m_exception( std::move( ex_ptr ) ) {
				m_keyvalues.emplace( "description", description.to_string() );
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

			Error& Error::add( boost::string_ref name, boost::string_ref value ) {
				if( m_frozen ) {
					throw std::runtime_error( "Attempt to change a frozen error." );
				}
				m_keyvalues[name.to_string( )] = value.to_string( );
				return *this;
			}

			boost::string_ref Error::get( boost::string_ref name ) const {
				return m_keyvalues.at( name.to_string( ) );
			}

			std::string & Error::get( boost::string_ref name ) {
				return m_keyvalues[name.to_string( )];
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

			std::string Error::to_string( boost::string_ref prefix ) const {
				std::stringstream ss;
				ss << "Description: " << m_keyvalues.at( "description" ) << "\n";
				for( auto const & row : m_keyvalues ) {
					if( row.first.compare( "description" ) != 0 ) {
						ss << prefix << "'" << row.first << "',	'" << row.second << "'\n";
					}
				}
				if( m_exception ) {
					try {
						std::rethrow_exception( m_exception );
					} catch( std::exception const & ex ) {
						ss << "Exception message: " << ex.what( ) << "\n";
					} catch( ... ) {
						ss << "Unknown exception\n";
					}
				}
				if( has_child( ) ) {
					ss << child( ).to_string( prefix.to_string() + "	" );
				}
				ss << "\n";
				return ss.str( );
			}

			std::ostream& operator<<(std::ostream& os, Error const & error) {
				os << error.to_string( );
				return os;
			}
			
			OptionalError create_optional_error( ) {
				return OptionalError( );
			}

	}	// namespace base
}	// namespace nodepp
}	// namespace daw
