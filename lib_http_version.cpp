#include <boost/lexical_cast.hpp>
#include <cstdint>
#include <string>
#include <utility>

#include "lib_http_version.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				namespace {
					std::pair<uint8_t, uint8_t> parse_string( std::string const & version ) {
						int major, minor;
						std::istringstream iss( version );
						iss >> major >> minor;	// TODO fix, doesn't account for . but assumes whitespace
						if( major < 0 && major > std::numeric_limits<uint8_t>::max( ) ) {
							throw std::invalid_argument( "Major version is out of range: " + version );
						} else if( minor < 0 && minor > std::numeric_limits<uint8_t>::max( ) ) {
							throw std::invalid_argument( "Minor version is out of range: " + version );
						}
						return{ major, minor };
					}
				}	// namespace anonymous

				uint8_t const & HttpVersion::major( ) const {
					return m_version.first;
				}

				uint8_t & HttpVersion::major( ) {
					return m_version.first;
				}

				uint8_t const & HttpVersion::minor( ) const {
					return m_version.second;
				}

				uint8_t & HttpVersion::minor( ) {
					return m_version.second;
				}

				HttpVersion::HttpVersion( ) : m_version( 0, 0 ), m_is_valid( false ) { }

				HttpVersion::HttpVersion( uint8_t const & Major, uint8_t const & Minor ) : m_version( Major, Minor ), m_is_valid( true ) { }

				HttpVersion::HttpVersion( std::string const & version ) : m_version( 0, 0 ), m_is_valid( true ) { 
					try {
						m_version = parse_string( version );
					} catch( std::exception const & ) {
						m_is_valid = false;
					}
				}

				HttpVersion& HttpVersion::operator=(std::string const & version) {
					m_version = parse_string( version );
					return *this;
				}

				HttpVersion& HttpVersion::operator=(HttpVersion && rhs) {
					if( this != &rhs ) {
						m_is_valid = std::move( rhs.m_is_valid );
						m_version = std::move( rhs.m_version );
					}
					return *this;
				}

				std::string HttpVersion::to_string( ) const {
					return std::to_string( major( ) ) + "." + std::to_string( minor( ) );
				}

				HttpVersion::operator std::string( ) const {
					return to_string( );
				}

				bool HttpVersion::is_valid( ) const {
					return m_is_valid;
				}
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
