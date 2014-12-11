#include <boost/regex.hpp>
#include <cstdint>
#include <string>
#include <vector>

#include "base_url.h"
#include "string.h"

namespace daw {
	namespace nodepp {
		namespace base {
			using namespace daw::nodepp;
			namespace {
				template<typename MatchResults>
				void add_if_matched( MatchResults match_results, std::vector<std::pair<uint16_t,uint16_t>> &result_cont, size_t item ) {
					if( match_results[item].matched ) {
						result_cont.emplace_back( static_cast<uint16_t>(match_results[item].first), tatic_cast<uint16_t>(match_results[item].second) );
					} else {
						result_cont.emplace_back( 0, 0 );
					}
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Parse a url into its component strings
				/// full url = 0
				/// protocol = 2
				/// host.domain = 4
				/// port = 6
				/// path = 7
				/// query = 9
				/// bookmark = 10
				/// {2}://{4}{5}?{7}#{9}
				template<typename Iterator>
				std::vector<std::pair<uint16_t, uint16_t>> parse_url( Iterator first, Iterator last ) {
					static std::regex const url_regex( R"(^(([^:/?#]+):)?(//([^/?:#]*))?(:(\d{1,}))?([^?#]*)(\?([^#]*))?(#(.*))?)" );
					//static boost::regex const url_regex( R"(^([^:/?#]+):?//([^/?:#]*)(:(\d{0,5}))?/?([^?#]*)\?([^#]*)?#(.*)?)" );
					boost::match_results<Iterator> url_parts;
					std::vector<std::string> result;
					if( boost::regex_search( url, url_parts, url_regex ) ) {
						assert( 12 == url_parts.size( ) );
						add_if_matched( url_parts, result, 2 );
						add_if_matched( url_parts, result, 4 );
						add_if_matched( url_parts, result, 6 );
						add_if_matched( url_parts, result, 7 );
						add_if_matched( url_parts, result, 9 );
						add_if_matched( url_parts, result, 11 );
					}
					return result;
				}
			}	// anonymous namespace 

			template<typename Iterator>
			void from_string( Iterator first, Iterator last ) {
				try {
					auto url_parts = parse_url( first, last );
					m_protocol = url_parts[0];
					m_host_name = url_parts[1];
					m_port = url_parts[2];
					m_path = url_parts[3];
					m_query = url_parts[4];
					m_hash = url_parts[5];
				} catch( std::exception const & ) {
					m_is_valid = false;
				}

			}


			Url::Url( std::string const & url ) : m_is_valid( true ) {
				from_string( url );
			}

			Url& Url::operator=(std::string const & url) {
				from_string( url );
				return *this;
			}

			Url::Url( std::string protocol, std::string host_port, std::string request_path ) : m_is_valid( true ) {
				from_string( protocol + "://" + host_port + request_path );
			}

			void Url::from_string( std::string const & url ) {
				from_string( url.cbegin( ), url.cend( ) );
			}

			bool Url::is_valid( ) const {
				return m_is_valid;
			}

			std::string Url::to_string( ) const {
				return daw::string::string_format( "{0}://{1}{2}{3}{4}{5}", m_protocol, m_host_name, m_port.empty( ) ? "" : ":" + m_port, m_path, m_query.empty( ) ? "" : "?" + m_query, m_hash.empty( ) ? "" : "#" + m_hash );
			}

		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
