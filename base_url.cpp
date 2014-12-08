#include <cstdint>
#include <regex>
#include <string>
#include <vector>

#include "base_url.h"
#include "string.h"

namespace daw {
	namespace nodepp {
		namespace base {
			using namespace daw::nodepp;
			namespace {
				//////////////////////////////////////////////////////////////////////////
				/// Summary: Parse a url into its component strings
				/// full url = 0
				/// protocol = 2
				/// host:port = 4
				/// path = 5
				/// query = 7
				/// bookmark = 9
				/// {2}://{4}{5}?{7}#{9}
				std::vector<std::string> parse_url( std::string const & url ) {
					static std::regex const url_regex( R"(^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?)" );
					std::smatch url_parts;
					std::vector<std::string> result( 10 );
					if( std::regex_match( url, url_parts, url_regex ) ) {
						assert( 10 == url_parts.size( ) );
						for( size_t i = 0; i < 10; i++ ) {
							result[i] = url_parts[i].str( );
						}
					}
					return result;
				}
			}	// anonymous namespace 

			Url::Url( std::string const & url ) {
				from_string( url );
			}

			Url& Url::operator=(std::string const & url) {
				from_string( url );
				return *this;
			}

			Url::Url( std::string protocol, std::string host_port, std::string request_path ) {
				from_string( protocol + "://" + host_port + request_path );
			}

			void Url::from_string( std::string const & url ) {
				auto url_parts = parse_url( url );
				m_protocol = url_parts[2];
				auto host_parts = daw::string::split( url_parts[4], ':' );
				m_host_name = host_parts[0];
				if( 1 < host_parts.size( ) ) {
					m_port = host_parts[1];
				}

				m_path = url_parts[5];
				m_query = url_parts[7];
				m_hash = url_parts[9];
			}


			std::string Url::to_string( ) const {
				return daw::string::string_format( "{0}://{1}{2}{3}{4}{5}", m_protocol, m_host_name, m_port.empty( ) ? "" : ":" + m_port, m_path, m_query.empty( ) ? "" : "?" + m_query, m_hash.empty() ? "": "#" + m_hash );
			}

		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
