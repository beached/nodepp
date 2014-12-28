#include <boost/utility/string_ref.hpp>
#include <boost/algorithm/string/split.hpp>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "lib_http.h"
#include "lib_http_site.h"
#include "string.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				namespace impl {
					using namespace daw::nodepp;

					bool site_registration::operator==(site_registration const & rhs) const {
						return rhs.host == host && rhs.path == path && rhs.method == method;
					}

					site_registration::site_registration( std::string Host, std::string Path, HttpClientRequestMethod Method, std::function < void( HttpClientRequest, HttpServerResponse ) > Listener ) :
						host( std::move( Host ) ),
						path( std::move( Path ) ),
						method( std::move( Method ) ),
						listener( std::move( Listener ) ) { }

					site_registration::site_registration( std::string Host, std::string Path, HttpClientRequestMethod Method ) :
						host( std::move( Host ) ),
						path( std::move( Path ) ),
						method( std::move( Method ) ),
						listener( nullptr ) { }

					HttpSiteImpl::HttpSiteImpl( base::EventEmitter emitter ) :
						m_emitter( std::move( emitter ) ),
						m_server( create_http_server( ) ),
						m_registered( ),
						m_error_listeners( ) { }

					HttpSiteImpl::HttpSiteImpl( HttpServer server, base::EventEmitter emitter ) :
						m_emitter( std::move( emitter ) ),
						m_server( std::move( server ) ),
						m_registered( ),
						m_error_listeners( ) { }

					void HttpSiteImpl::start( ) {
						m_server->on_error( get_weak_ptr( ), "Child" )
							.on_client_connected( [&]( HttpConnection connection ) {
							auto obj = get_weak_ptr( );
							connection->
								on_error( get_weak_ptr( ), "child connection" )
								.on_request_made( [obj]( HttpClientRequest request, HttpServerResponse response ) {
									run_if_valid( obj, "Processing request", "HttpSiteImpl::start( )#on_request_made", [&request, &response]( HttpSite self ) {
										auto host = [&]( ) {
											auto host_it = request->headers.find( "Host" );
											if( request->headers.end( ) == host_it || "" == host_it->second ) {
												return std::string( );
											}
											auto result = daw::string::split( host_it->second, ':' );
											if( 0 < result.size( ) && result.size( ) <= 2 ) {
												return result[0];
											}
											return std::string( );
										}();
										if( "" == host ) {
											self->emit_page_error( request, response, 400 );
										} else {
											auto site = self->match( host, request->request.url, request->request.method );
											if( self->end( ) == site ) {
												self->emit_page_error( request, response, 404 );
											} else {
												site->listener( request, response );
											}
										}
									} );
							} );
						} );
					}

					base::EventEmitter& HttpSiteImpl::emitter( ) {
						return m_emitter;
					}

					void HttpSiteImpl::sort_registered( ) {
						daw::algorithm::sort( m_registered, []( site_registration const & lhs, site_registration const & rhs ) {
							return lhs.host < rhs.host;
						} );

						daw::algorithm::stable_sort( m_registered, []( site_registration const & lhs, site_registration const & rhs ) {
							return lhs.path < rhs.path;
						} );
					}

					HttpSiteImpl& HttpSiteImpl::on_requests_for( HttpClientRequestMethod method, std::string path, std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
						m_registered.emplace_back( "*", std::move( path ), std::move( method ), listener );
						return *this;
					}

					HttpSiteImpl& HttpSiteImpl::on_requests_for( std::string hostname, HttpClientRequestMethod method, std::string path, std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
						m_registered.emplace_back( std::move( hostname ), std::move( hostname ), std::move( method ), listener );
						return *this;
					}

					void HttpSiteImpl::remove( HttpSiteImpl::iterator item ) {
						m_registered.erase( item );
					}

					HttpSiteImpl::iterator HttpSiteImpl::end( ) {
						return m_registered.end( );
					}

					HttpSiteImpl::iterator HttpSiteImpl::match( boost::string_ref host, boost::string_ref path, HttpClientRequestMethod method ) {
						auto key = site_registration( host.to_string( ), path.to_string( ), method );
						iterator result = std::find_if( m_registered.begin( ), m_registered.end( ), [&key]( site_registration const & item ) {
							return (key.host == "*" || item.host == "*" || key.host == item.host) &&
								key.path == item.path &&
								(key.method == HttpClientRequestMethod::Any || item.method == HttpClientRequestMethod::Any || key.method == item.method);
						} );
						return result;
					}

					bool HttpSiteImpl::has_error_handler( uint16_t error_no ) {
						return std::end( m_error_listeners ) == m_error_listeners.find( error_no );
					}

					HttpSiteImpl& HttpSiteImpl::clear_page_error_listeners( ) {
						m_error_listeners.clear( );
						return *this;
					}

					HttpSiteImpl& HttpSiteImpl::on_any_page_error( std::function < void( HttpClientRequest, HttpServerResponse, uint16_t error_no ) > listener ) {
						m_error_listeners[0] = listener;
						return *this;
					}

					HttpSiteImpl& HttpSiteImpl::except_on_page_error( uint16_t error_no ) {
						m_error_listeners.erase( error_no );
						return *this;
					}

					HttpSiteImpl& HttpSiteImpl::on_page_error( uint16_t error_no, std::function < void( HttpClientRequest, HttpServerResponse, uint16_t error_no ) > listener ) {
						m_error_listeners[error_no] = std::move( listener );
						return *this;
					}

					void HttpSiteImpl::default_page_error_listener( HttpClientRequest request, HttpServerResponse response, uint16_t error_no ) {
						auto msg = HttpStatusCodes( error_no );
						if( msg.first != error_no ) {
							msg.first = error_no;
							msg.second = "Error";
						}
						response->send_status( msg.first, msg.second );
						response->end( "<html><body><h2>" + std::to_string( msg.first ) + " " + msg.second + "</h2>\r\n</body></html>\r\n" );
					}

					void HttpSiteImpl::emit_page_error( HttpClientRequest request, HttpServerResponse response, uint16_t error_no ) {
						auto err_it = m_error_listeners.find( error_no );
						std::function < void( HttpClientRequest, HttpServerResponse, uint16_t )> handler = std::bind( &HttpSiteImpl::default_page_error_listener, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );

						if( std::end( m_error_listeners ) != err_it ) {
							handler = err_it->second;
						} else if( std::end( m_error_listeners ) != (err_it = m_error_listeners.find( 0 )) ) {
							handler = err_it->second;
						}
						handler( request, response, error_no );
					}

					HttpSiteImpl& HttpSiteImpl::listen_on( uint16_t port ) {
						m_server->listen_on( port );
						return *this;
					}

				}	// namespace impl

				HttpSite create_http_site( base::EventEmitter emitter ) {
					auto result = HttpSite( new impl::HttpSiteImpl( std::move( emitter ) ) );
					result->start( );
					return result;
				}

				HttpSite create_http_site( HttpServer server, base::EventEmitter emitter ) {
					auto result = HttpSite( new impl::HttpSiteImpl( std::move( server ), std::move( emitter ) ) );
					result->start( );
					return result;
				}

			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
