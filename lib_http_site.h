#pragma once

#include <boost/utility/string_ref.hpp>
#include <string>
#include <utility>
#include <vector>

#include "base_event_emitter.h"
#include "lib_http_request.h"
#include "lib_http_server_response.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				namespace impl {					
					using namespace daw::nodepp;
					struct site_registration {
						std::string host;	// * = any
						std::string path;	// postfixing with a * means match left
						std::string method;
					};

					class HttpSiteImpl: public base::enable_shared<HttpSiteImpl>, public base::StandardEvents <HttpSiteImpl> {
					public:
						using registered_items_t = std::vector < site_registration > ;
						using iterator = registered_items_t::iterator;
					
					private:
						base::EventEmitter m_emitter;
						std::vector<site_registration> m_registered;

						void sort_registered( );

					public:
						HttpSiteImpl( base::EventEmitter emitter );
						iterator create( lib::http::HttpRequestMethod method, boost::string_ref path );
						iterator create( boost::string_ref hostname, lib::http::HttpRequestMethod method, boost::string_ref path);

						bool remove( iterator item );

						iterator end( );
						iterator best_match( boost::string_ref host, boost::string_ref path, lib::http::HttpRequestMethod method );

					};	// class HttpSiteImpl
				}	// namespace impl
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw