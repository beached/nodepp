
#include "lib_http_site.h"
#include "lib_http.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				namespace impl {
					using namespace daw::nodepp;

					HttpSiteImpl::HttpSiteImpl( base::EventEmitter emitter ):
						m_emitter( std::move( emitter ) ), 
						m_server( create_http_server( ) ), 
						m_registered( ), 
						m_error_listeners( ) { }

					HttpSiteImpl::HttpSiteImpl( HttpServer server, base::EventEmitter emitter ) :
						m_emitter( std::move( emitter ) ), 
						m_server( std::move( server ) ), 
						m_registered( ), 
						m_error_listeners( ) { }
					
					void HttpSiteImpl::set_server_listeners( ) {
						m_server->
					}

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

					void HttpSiteImpl::remove( HttpSiteImpl::iterator item ) {
						m_registered.erase( item );
					}

					HttpSiteImpl::iterator HttpSiteImpl::end( ) {
						return m_registered.end( );
					}
					
					HttpSiteImpl::iterator HttpSiteImpl::best_match( boost::string_ref host, boost::string_ref path, lib::http::HttpRequestMethod method ) {
						throw std::runtime_error( "Method not implemented" );
					}

					HttpSiteImpl::iterator HttpSiteImpl::best_match( boost::string_ref host, uint16_t error_code ) {
						throw std::runtime_error( "Method not implemented" );
					}

					bool HttpSiteImpl::has_error_handler( uint16_t error_no ) {
						return std::end( m_error_listeners ) == m_error_listeners.find( error_no );
					}

					HttpSiteImpl& HttpSiteImpl::clear_page_error_listeners( ) {
						throw std::runtime_error( "Method not implemented" );
					}

					HttpSiteImpl& HttpSiteImpl::on_any_page_error( std::function < void( lib::http::HttpClientRequest, lib::http::HttpServerResponse, uint16_t error_no ) > listener ) {
						m_error_listeners[0] = listener;
						return *this;
					}

					HttpSiteImpl& HttpSiteImpl::except_on_page_error( uint16_t error_no ) {
						throw std::runtime_error( "Method not implemented" );
						return *this;
					}

					HttpSiteImpl& HttpSiteImpl::on_page_error( uint16_t error_no, std::function < void( lib::http::HttpClientRequest, lib::http::HttpServerResponse, uint16_t error_no ) > listener ) {
						throw std::runtime_error( "Method not implemented" );
						return *this;
					}

					void HttpSiteImpl::default_page_error_listener( lib::http::HttpClientRequest request, lib::http::HttpServerResponse response, uint16_t error_no ) {
						auto msg = lib::http::HttpStatusCodes( error_no );
						if( msg.first != error_no ) {
							msg.first = error_no;
							msg.second = "Error";
						}
						response->send_status( msg.first, msg.second );
						response->end( "<html><body><h2>" + msg.second + "</h2>\r\n</body></html>\r\n" );
					}

					void HttpSiteImpl::emit_page_error( lib::http::HttpClientRequest request, lib::http::HttpServerResponse response, uint16_t error_no ) {
						auto err_it = m_error_listeners.find( error_no );
						std::function < void( lib::http::HttpClientRequest, lib::http::HttpServerResponse, uint16_t )> handler = std::bind( &HttpSiteImpl::default_page_error_listener, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );

						if( std::end( m_error_listeners ) != err_it ) {
							handler = err_it->second;
						} else if( std::end( m_error_listeners ) != (err_it = m_error_listeners.find( 0 )) ) {
							handler = err_it->second;
						}
						handler( request, response, error_no );
					}

				}	// namespace impl
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw