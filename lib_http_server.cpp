#include <boost/shared_ptr.hpp>
#include <cinttypes>
#include <iterator>
#include <string>
#include <utility>

#include "base_event_emitter.h"
#include "base_service_handle.h"
#include "lib_http_connection.h"
#include "lib_http_server.h"
#include "lib_net_server.h"
#include "utility.h"
#include "range_algorithm.h"
#include <cstdlib>

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				namespace impl {

					using namespace daw::nodepp;

					HttpServerImpl::HttpServerImpl( base::EventEmitter emitter ) :
						m_netserver( lib::net::create_net_server( ) ),
						m_emitter( emitter ),
						m_connections( ) { }

					HttpServerImpl::HttpServerImpl( HttpServerImpl&& other ) :
						m_netserver( std::move( other.m_netserver ) ),
						m_emitter( std::move( other.m_emitter ) ),
						m_connections( std::move( other.m_connections ) ) { }

					HttpServerImpl& HttpServerImpl::operator=(HttpServerImpl && rhs) {
						if( this != &rhs ) {
							m_netserver = std::move( rhs.m_netserver );
							m_emitter = std::move( rhs.m_emitter );
							m_connections = std::move( rhs.m_connections );
						}
						return *this;
					}

					std::shared_ptr<HttpServerImpl> HttpServerImpl::get_ptr( ) {
						return shared_from_this( );
					}

					base::EventEmitter& HttpServerImpl::emitter( ) {
						return m_emitter;
					}

					void HttpServerImpl::emit_client_connected( HttpConnection connection ) {
						emitter( )->emit( "client_connected", connection );
					}

					void HttpServerImpl::emit_closed( ) {
						emitter( )->emit( "closed" );
					}

					void HttpServerImpl::emit_listening( boost::asio::ip::tcp::endpoint endpoint ) {
						emitter( )->emit( "listening", std::move( endpoint ) );
					}

					void HttpServerImpl::handle_connection( std::weak_ptr<HttpServerImpl> obj, lib::net::NetSocketStream socket ) {
						run_if_valid( obj, "Exception while connecting", "HttpServerImpl::handle_connection", [&]( std::shared_ptr<HttpServerImpl>& self ) {
							auto connection = create_http_connection( std::move( socket ) );
							auto it = self->m_connections.emplace( self->m_connections.end( ), connection );

							connection->on_error( [obj]( base::Error error ) mutable {
								if( !obj.expired( ) ) {
									auto self_l = obj.lock( );
									self_l->emit_error( "HttpServerImpl::handle_connection", std::move( error ) );
								}
							} ).on_closed( [it, obj] ( ) mutable { 
								if( !obj.expired( ) ) {
									auto self_l = obj.lock( );
									try {
										self_l->m_connections.erase( it );
									} catch( ... ) {
										self_l->emit_error( std::current_exception( ), "Could not delete connection", "HttpServerImpl::handle_connection" );
									}
								}
							} ).start( );

							try {
								self->emit_client_connected( connection );								
							} catch( ... ) {
								self->emit_error( std::current_exception( ), "Running connection listeners", "HttpServerImpl::handle_connection" );
							}
						} );
					}

					void HttpServerImpl::handle_error( std::weak_ptr<HttpServerImpl> obj, base::Error error ) {
						if( !obj.expired( ) ) {
							obj.lock( )->emit_error( "HttpServerImpl::handle_connection", std::move( error ) );
						}
					}

					void HttpServerImpl::listen_on( uint16_t port ) {
						std::weak_ptr<HttpServerImpl> obj = get_ptr( );
						m_netserver->on_connection( [obj]( lib::net::NetSocketStream socket ) {
							if( !obj.expired( ) ) {
								obj.lock( )->handle_connection( obj, std::move( socket ) );
							}
						} ).on_error( [obj]( base::Error error ) {
							if( !obj.expired( ) ) {
								obj.lock( )->handle_error( obj, std::move( error ) );
							}
						} ).on_listening( [obj]( boost::asio::ip::tcp::endpoint endpoint ) {
							if( !obj.expired( ) ) {
								obj.lock( )->emit_listening( std::move( endpoint ) );
							}
						} ).listen( port );
					}

					void HttpServerImpl::listen_on( uint16_t, std::string, uint16_t ) { throw std::runtime_error( "Method not implemented" ); }

					void HttpServerImpl::listen_on( std::string ) { throw std::runtime_error( "Method not implemented" ); }

					void HttpServerImpl::listen_on( base::ServiceHandle ) { throw std::runtime_error( "Method not implemented" ); }


					size_t& HttpServerImpl::max_header_count( ) { throw std::runtime_error( "Method not implemented" ); }
					size_t const & HttpServerImpl::max_header_count( ) const { throw std::runtime_error( "Method not implemented" ); }

					size_t HttpServerImpl::timeout( ) const { throw std::runtime_error( "Method not implemented" ); }

					HttpServerImpl& HttpServerImpl::on_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener ) {
						emitter( )->add_listener( "listening", listener );
						return *this;
					}

					HttpServerImpl& HttpServerImpl::on_next_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener ) {
						emitter( )->add_listener( "listening", listener, true );
						return *this;
					}

					HttpServerImpl& HttpServerImpl::on_client_connected( std::function<void( HttpConnection )> listener ) {
						emitter( )->add_listener( "client_connected", listener );
						return *this;
					}

					HttpServerImpl& HttpServerImpl::on_next_client_connected( std::function<void( HttpConnection )> listener ) {
						emitter( )->add_listener( "client_connected", listener, true );
						return *this;
					}

					HttpServerImpl& HttpServerImpl::on_closed( std::function<void( )> listener ) {
						emitter( )->add_listener( "closed", listener );
						return *this;
					}

					HttpServerImpl& HttpServerImpl::on_next_closed( std::function<void( )> listener ) {
						emitter( )->add_listener( "closed", listener, true );
						return *this;
					}


				}	// namespace impl

				HttpServer create_http_server( base::EventEmitter emitter ) {
					return HttpServer( new impl::HttpServerImpl( std::move( emitter ) ) );
				}

			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
