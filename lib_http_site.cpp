
#include "lib_http_site.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				namespace impl {
					using namespace daw::nodepp;

					HttpSiteImpl::HttpSiteImpl( base::EventEmitter emitter ) { }
					
					void HttpSiteImpl::sort_registered( ) {
						daw::algorithm::sort( m_registered, []( HttpSiteImpl::registered_page_t const & lhs, HttpSiteImpl::registered_page_t const & rhs ) {
							return lhs.first.host < rhs.first.host;
						} );
						
						daw::algorithm::stable_sort( m_registered, []( HttpSiteImpl::registered_page_t const & lhs, HttpSiteImpl::registered_page_t const & rhs ) {
							return lhs.first.path < rhs.first.path;
						} );
					}
									

					HttpSiteImpl::iterator HttpSiteImpl::create_path( lib::http::HttpRequestMethod method, std::string path, HttpSiteImpl::page_listener_t listener ) {
						return m_registered.emplace( m_registered.end( ), site_registration( "*", std::move( path ), std::move( method ) ), listener );
					}
					HttpSiteImpl::iterator HttpSiteImpl::create_path( std::string hostname, lib::http::HttpRequestMethod method, std::string path, HttpSiteImpl::page_listener_t listener ) {
						return m_registered.emplace( m_registered.end( ), site_registration( std::move( hostname ), std::move( hostname ), std::move( method ) ), listener );
					}

					void HttpSiteImpl::remove( iterator item ) {
						m_registered.erase( item );
					}

					HttpSiteImpl::iterator HttpSiteImpl::end( ) {
						return m_registered.end( );
					}
					
					HttpSiteImpl::iterator HttpSiteImpl::best_match( boost::string_ref host, boost::string_ref path, lib::http::HttpRequestMethod method ) {
						return m_registered.end( );
					}

					HttpSiteImpl::iterator HttpSiteImpl::best_match( boost::string_ref host, uint16_t error_code ) {
						return m_registered.end( );
					}


				}	// namespace impl
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw