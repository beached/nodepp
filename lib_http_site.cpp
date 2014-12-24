
#include "lib_http_site.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				namespace impl {
					using namespace daw::nodepp;

					HttpSiteImpl::HttpSiteImpl( base::EventEmitter emitter ) { }
					
					void HttpSiteImpl::sort_registered( ) {
						daw::algorithm::sort( m_registered, []( site_registration const & lhs, site_registration const & rhs ) {
							return lhs.host < rhs.host;
						} );
						
						daw::algorithm::stable_sort( m_registered, []( site_registration const & lhs, site_registration const & rhs ) {
							return lhs.path < rhs.path;
						} );
					}
					
					void HttpSiteImpl::create(lib::http::HttpRequestMethod method, boost::string_ref path) { }
					void HttpSiteImpl::create(boost::string_ref hostname, lib::http::HttpRequestMethod method, boost::string_ref path) { }



				}	// namespace impl
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw