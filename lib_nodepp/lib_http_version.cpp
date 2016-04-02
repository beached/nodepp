// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <boost/lexical_cast.hpp>
#include <cstdint>
#include <string>
#include <utility>

#include "lib_http_version.h"
#include <boost/utility/string_ref.hpp>

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				namespace {
					std::pair<uint8_t, uint8_t> parse_string( boost::string_ref version ) {
						int major, minor;
						std::istringstream iss( version.to_string( ) );
						iss>> major>> minor;	// TODO: fix, doesn't account for . but assumes whitespace
						if( major <0 && major> std::numeric_limits<uint8_t>::max( ) ) {
							throw std::invalid_argument( "Major version is out of range: " + version.to_string( ) );
						} else if( minor <0 && minor> std::numeric_limits<uint8_t>::max( ) ) {
							throw std::invalid_argument( "Minor version is out of range: " + version.to_string( ) );
						}
						return { major, minor };
					}
				}	// namespace anonymous

				uint_fast8_t const & HttpVersion::major( ) const {
					return m_version.first;
				}

				uint_fast8_t & HttpVersion::major( ) {
					return m_version.first;
				}

				uint_fast8_t const & HttpVersion::minor( ) const {
					return m_version.second;
				}

				uint_fast8_t & HttpVersion::minor( ) {
					return m_version.second;
				}

				HttpVersion::HttpVersion( ): m_version( 0, 0 ), m_is_valid( false ) { }

				HttpVersion::HttpVersion( uint_fast8_t Major, uint_fast8_t Minor ) : m_version( Major, Minor ), m_is_valid( true ) { }

				HttpVersion::HttpVersion( boost::string_ref version ) : m_version( 0, 0 ), m_is_valid( true ) {
					try {
						m_version = parse_string( version );
					} catch( std::exception const & ) {
						m_is_valid = false;
					}
				}

				HttpVersion& HttpVersion::operator=( boost::string_ref version ) {
					m_version = parse_string( version );
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

