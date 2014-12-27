#pragma once

#include <boost/utility/string_ref.hpp>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "base_event_emitter.h"
#include "lib_http_request.h"
#include "lib_http_server_response.h"
#include "lib_http_server.h"

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
						HttpServer m_server;
						registered_pages_t m_registered;
						std::unordered_map<uint16_t, std::function < void( lib::http::HttpClientRequest, lib::http::HttpServerResponse, uint16_t ) >> m_error_listeners;

						void sort_registered( );
						void default_page_error_listener( lib::http::HttpClientRequest request, lib::http::HttpServerResponse response, uint16_t error_no );
						void set_server_listeners( );
					public:
						HttpSiteImpl( base::EventEmitter emitter );
						HttpSiteImpl( HttpServer server, base::EventEmitter emitter );
						iterator create_path( lib::http::HttpRequestMethod method, std::string path, page_listener_t listener );
						iterator create_path( std::string hostname, lib::http::HttpRequestMethod method, std::string path, page_listener_t listener );

						void remove( iterator item );

						iterator end( );
						iterator best_match( boost::string_ref host, boost::string_ref path, lib::http::HttpRequestMethod method );
						iterator best_match( boost::string_ref host, uint16_t error_code );

						bool has_error_handler( uint16_t error_no );
						
						//////////////////////////////////////////////////////////////////////////
						// Summary:	Use the default error handler for HTTP errors. This is the
						//			default.
						HttpSiteImpl& clear_page_error_listeners( );

						//////////////////////////////////////////////////////////////////////////
						// Summary:	Create a generic error handler
						HttpSiteImpl& on_any_page_error( std::function < void( lib::http::HttpClientRequest, lib::http::HttpServerResponse, uint16_t error_no ) > listener );

						//////////////////////////////////////////////////////////////////////////
						// Summary:	Use the default error handler for specific HTTP error.
						HttpSiteImpl& except_on_page_error( uint16_t error_no );

						//////////////////////////////////////////////////////////////////////////
						// Summary:	Specify a callback to handle a specific page error
						HttpSiteImpl& on_page_error( uint16_t error_no, std::function < void( lib::http::HttpClientRequest, lib::http::HttpServerResponse, uint16_t error_no ) > listener );
						
						
						void emit_page_error( lib::http::HttpClientRequest request, lib::http::HttpServerResponse response, uint16_t error_no );
						void emit_listening( boost::asio::ip::tcp::endpoint endpoint );

						HttpSiteImpl& on_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener );
						HttpSiteImpl& listen( uint16_t port );
					};	// class HttpSiteImpl
				}	// namespace impl
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw