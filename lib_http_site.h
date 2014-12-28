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
					class HttpSiteImpl;
				}	// namespace impl

				using HttpSite = std::shared_ptr < impl::HttpSiteImpl > ;

				HttpSite create_http_site( base::EventEmitter emitter = base::create_event_emitter( ) );
				HttpSite create_http_site( HttpServer server, base::EventEmitter emitter = base::create_event_emitter( ) );

				namespace impl {					
					using namespace daw::nodepp;
					using namespace lib::http;

					struct site_registration {
						std::string host;	// * = any
						std::string path;	// postfixing with a * means match left
						HttpClientRequestMethod method;
						std::function < void( HttpClientRequest, HttpServerResponse ) > listener;

						site_registration( ) = default;
						site_registration( site_registration const & ) = default;
						site_registration& operator=(site_registration const &) = default;
						~site_registration( ) = default;

						bool operator==(site_registration const & rhs) const;

						site_registration( std::string Host, std::string Path, HttpClientRequestMethod Method );
						site_registration( std::string Host, std::string Path, HttpClientRequestMethod Method, std::function < void( HttpClientRequest, HttpServerResponse ) > Listener );
					};	// site_registration

					class HttpSiteImpl: public base::enable_shared<HttpSiteImpl>, public base::StandardEvents <HttpSiteImpl> {
					public:
						using registered_pages_t = std::vector <site_registration> ;
						using iterator = registered_pages_t::iterator;
					
					private:
						base::EventEmitter m_emitter;
						HttpServer m_server;
						registered_pages_t m_registered;
						std::unordered_map<uint16_t, std::function < void( HttpClientRequest, HttpServerResponse, uint16_t ) >> m_error_listeners;

						void sort_registered( );
						void default_page_error_listener( HttpClientRequest request, HttpServerResponse response, uint16_t error_no );
						void start( );
						HttpSiteImpl( base::EventEmitter emitter );
						HttpSiteImpl( HttpServer server, base::EventEmitter emitter );
					public:
						friend HttpSite lib::http::create_http_site( base::EventEmitter );
						friend HttpSite lib::http::create_http_site( HttpServer, base::EventEmitter );

						HttpSiteImpl( HttpSiteImpl const & ) = delete;
						HttpSiteImpl& operator=(HttpSiteImpl const &) = delete;	
						HttpSiteImpl( HttpSiteImpl&& ) = delete;
						HttpSiteImpl& operator=(HttpSiteImpl&&) = delete;
						~HttpSiteImpl( ) = default;
						//////////////////////////////////////////////////////////////////////////
						/// Summary:	Register a listener for a HTTP method and path on any
						///				host
						HttpSiteImpl& create_path( HttpClientRequestMethod method, std::string path, std::function<void( HttpClientRequest, HttpServerResponse )> listener );

						//////////////////////////////////////////////////////////////////////////
						/// Summary:	Register a listener for a HTTP method and path on a 
						///				specific hostname
						HttpSiteImpl& create_path( std::string hostname, HttpClientRequestMethod method, std::string path, std::function<void( HttpClientRequest, HttpServerResponse )> listener );

						base::EventEmitter& emitter( );

						void remove( iterator item );

						iterator end( );
						iterator best_match( boost::string_ref host, boost::string_ref path, HttpClientRequestMethod method );

						bool has_error_handler( uint16_t error_no );
						
						//////////////////////////////////////////////////////////////////////////
						// Summary:	Use the default error handler for HTTP errors. This is the
						//			default.
						HttpSiteImpl& clear_page_error_listeners( );

						//////////////////////////////////////////////////////////////////////////
						// Summary:	Create a generic error handler
						HttpSiteImpl& on_any_page_error( std::function < void( HttpClientRequest, HttpServerResponse, uint16_t error_no ) > listener );

						//////////////////////////////////////////////////////////////////////////
						// Summary:	Use the default error handler for specific HTTP error.
						HttpSiteImpl& except_on_page_error( uint16_t error_no );

						//////////////////////////////////////////////////////////////////////////
						// Summary:	Specify a callback to handle a specific page error
						HttpSiteImpl& on_page_error( uint16_t error_no, std::function < void( HttpClientRequest, HttpServerResponse, uint16_t error_no ) > listener );
						
						
						void emit_page_error( HttpClientRequest request, HttpServerResponse response, uint16_t error_no );
						void emit_listening( boost::asio::ip::tcp::endpoint endpoint );

						HttpSiteImpl& on_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener );
						HttpSiteImpl& listen_on( uint16_t port );
					};	// class HttpSiteImpl
				}	// namespace impl
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw