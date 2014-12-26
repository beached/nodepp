#pragma once

#include <boost/utility/string_ref.hpp>
#include <cstdint>
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
						lib::http::HttpRequestMethod method;
						site_registration( ) = default;
						site_registration( site_registration const & ) = default;
						site_registration& operator=(site_registration const &) = default;
						~site_registration( ) = default;
						site_registration( std::string Host, std::string Path, lib::http::HttpRequestMethod Method ) : 
							host( std::move( Host ) ),
							path( std::move( Path ) ),
							method( std::move( Method ) ) { }
					};

					class HttpSiteImpl: public base::enable_shared<HttpSiteImpl>, public base::StandardEvents <HttpSiteImpl> {
					public:
						using page_listener_t = std::function < void( lib::http::HttpClientRequest, lib::http::HttpServerResponse ) > ;
						using registered_page_t = std::pair < site_registration, page_listener_t > ;
						using registered_pages_t = std::vector <registered_page_t> ;
						using iterator = registered_pages_t::iterator;
					
					private:
						base::EventEmitter m_emitter;
						registered_pages_t m_registered;

						void sort_registered( );

					public:
						HttpSiteImpl( base::EventEmitter emitter );
						iterator create_path( lib::http::HttpRequestMethod method, std::string path, page_listener_t listener );
						iterator create_path( std::string hostname, lib::http::HttpRequestMethod method, std::string path, page_listener_t listener );

						iterator set_error_page( uint16_t err_code, page_listener_t callback );

						void remove( iterator item );

						iterator end( );
						iterator best_match( boost::string_ref host, boost::string_ref path, lib::http::HttpRequestMethod method );
						iterator best_match( boost::string_ref host, uint16_t error_code );

					};	// class HttpSiteImpl
				}	// namespace impl
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw