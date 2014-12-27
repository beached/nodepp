
#include "lib_http_site.h"
#include "lib_http.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				namespace impl {
					using namespace daw::nodepp;

					HttpSiteImpl::HttpSiteImpl( base::EventEmitter emitter ) :
						m_emitter( std::move( emitter ) ),
						m_server( create_http_server( ) ),
						m_registered( ),
						m_error_listeners( ) {
						set_server_listeners( );
					}

					HttpSiteImpl::HttpSiteImpl( HttpServer server, base::EventEmitter emitter ) :
						m_emitter( std::move( emitter ) ),
						m_server( std::move( server ) ),
						m_registered( ),
						m_error_listeners( ) {
						set_server_listeners( );
					}

					void HttpSiteImpl::set_server_listeners( ) {
						m_server->on_error( get_weak_ptr( ), "Child" ).on_client_connected( [&]( HttpConnection connection ) {
							connection->
								on_error( get_weak_ptr( ), "child connection" );// .
// 								on_request_made( [&]( HttpClientRequest request, HttpServerResponse response ) {
// 									auto host = [&]( ) {
// 										auto host_it = request->headers.find( "host" );
// 										if( request->headers.end( ) == host_it ) {
// 											return std::string( "*" );
// 										}
// 										return host_it->second;
// 									}();
// 									auto listener = best_match( host, request->request.url, request->request.method );
// 									if( end( ) == listener ) {
// 										return;
// 									}
// 									listener->second( *request, response );
// 								} );
						} );
					}

					void HttpSiteImpl::sort_registered( ) {
						daw::algorithm::sort( m_registered, []( HttpSiteImpl::registered_page_t const & lhs, HttpSiteImpl::registered_page_t const & rhs ) {
							return lhs.first.host < rhs.first.host;
						} );

						daw::algorithm::stable_sort( m_registered, []( HttpSiteImpl::registered_page_t const & lhs, HttpSiteImpl::registered_page_t const & rhs ) {
							return lhs.first.path < rhs.first.path;
						} );
					}

					HttpSiteImpl::iterator HttpSiteImpl::create_path( HttpClientRequestMethod method, std::string path, HttpSiteImpl::page_listener_t listener ) {
						return m_registered.emplace( m_registered.end( ), site_registration( "*", std::move( path ), std::move( method ) ), listener );
					}
					HttpSiteImpl::iterator HttpSiteImpl::create_path( std::string hostname, HttpClientRequestMethod method, std::string path, HttpSiteImpl::page_listener_t listener ) {
						return m_registered.emplace( m_registered.end( ), site_registration( std::move( hostname ), std::move( hostname ), std::move( method ) ), listener );
					}

					void HttpSiteImpl::remove( HttpSiteImpl::iterator item ) {
						m_registered.erase( item );
					}

					HttpSiteImpl::iterator HttpSiteImpl::end( ) {
						return m_registered.end( );
					}

					HttpSiteImpl::iterator HttpSiteImpl::best_match( boost::string_ref host, boost::string_ref path, HttpClientRequestMethod method ) {
						throw std::runtime_error( "Method not implemented" );
					}

					bool HttpSiteImpl::has_error_handler( uint16_t error_no ) {
						return std::end( m_error_listeners ) == m_error_listeners.find( error_no );
					}

					HttpSiteImpl& HttpSiteImpl::clear_page_error_listeners( ) {
						m_error_listeners.clear( );
						return *this;
					}

					HttpSiteImpl& HttpSiteImpl::on_any_page_error( std::function < void( HttpClientRequestImpl, HttpServerResponse, uint16_t error_no ) > listener ) {
						m_error_listeners[0] = listener;
						return *this;
					}

					HttpSiteImpl& HttpSiteImpl::except_on_page_error( uint16_t error_no ) {
						m_error_listeners.erase( error_no );
						return *this;
					}

					HttpSiteImpl& HttpSiteImpl::on_page_error( uint16_t error_no, std::function < void( HttpClientRequestImpl, HttpServerResponse, uint16_t error_no ) > listener ) {
						m_error_listeners[error_no] = std::move( listener );
						return *this;
					}

					void HttpSiteImpl::default_page_error_listener( HttpClientRequestImpl request, HttpServerResponse response, uint16_t error_no ) {
						auto msg = HttpStatusCodes( error_no );
						if( msg.first != error_no ) {
							msg.first = error_no;
							msg.second = "Error";
						}
						response->send_status( msg.first, msg.second );
						response->end( "<html><body><h2>" + msg.second + "</h2>\r\n</body></html>\r\n" );
					}

					void HttpSiteImpl::emit_page_error( HttpClientRequestImpl request, HttpServerResponse response, uint16_t error_no ) {
						auto err_it = m_error_listeners.find( error_no );
						std::function < void( HttpClientRequestImpl, HttpServerResponse, uint16_t )> handler = std::bind( &HttpSiteImpl::default_page_error_listener, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );

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