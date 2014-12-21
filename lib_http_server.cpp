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
						base::StandardEvents<HttpServerImpl>( emitter ),
						m_netserver( ),
						m_emitter( emitter ) { }

					HttpServerImpl::HttpServerImpl( HttpServerImpl&& other ) :
						base::StandardEvents<HttpServerImpl>( std::move( other ) ),
						m_netserver( std::move( other.m_netserver ) ),
						m_emitter( std::move( other.m_emitter ) ) { }

					HttpServerImpl& HttpServerImpl::operator=(HttpServerImpl && rhs) {
						if( this != &rhs ) {
							m_netserver = std::move( rhs.m_netserver );
							m_emitter = std::move( rhs.m_emitter );
						}
						return *this;
					}

					std::shared_ptr<HttpServerImpl> HttpServerImpl::get_ptr( ) {
						return shared_from_this( );
					}

					void HttpServerImpl::emit_connection( HttpConnection connection ) {
						m_emitter->emit( "connection", connection );
					}

					void HttpServerImpl::emit_close( ) {
						m_emitter->emit( "close" );
					}

					void HttpServerImpl::emit_listening( boost::asio::ip::tcp::endpoint endpoint ) {
						m_emitter->emit( "listening", std::move( endpoint ) );
					}

					void HttpServerImpl::handle_connection( std::shared_ptr<HttpServerImpl> self, lib::net::NetSocketStream socket ) {
						auto connection = create_http_connection( std::move( socket ) );

						connection->on_error( [&]( base::Error error ) {
							self->emit_error( "HttpServerImpl::handle_connection", std::move( error ) );
						} );

						try {
							self->emit_connection( connection->get_ptr( ) );
						} catch( ... ) {
							self->emit_error( std::current_exception( ), "Running connection listeners", "HttpServerImpl::handle_connection" );
						}
					}

					void HttpServerImpl::handle_error( std::shared_ptr<HttpServerImpl> self, base::Error error ) {
						self->emit_error( "HttpServerImpl::handle_connection", std::move( error ) );
					}

					void HttpServerImpl::listen_on( uint16_t port ) {
						m_netserver.on_connection( [&]( lib::net::NetSocketStream socket ) {
							handle_connection( get_ptr( ), std::move( socket ) );
						} );

						m_netserver.on_error( [&]( base::Error error ) {
							handle_error( get_ptr( ), std::move( error ) );
						} );

						m_netserver.on_listening( [&]( boost::asio::ip::tcp::endpoint endpoint ) {
							emit_listening( std::move( endpoint ) );
						} );

						m_netserver.listen( port );
					}

					void HttpServerImpl::listen_on( uint16_t, std::string, uint16_t ) { throw std::runtime_error( "Method not implemented" ); }

					void HttpServerImpl::listen_on( std::string ) { throw std::runtime_error( "Method not implemented" ); }

					void HttpServerImpl::listen_on( base::ServiceHandle ) { throw std::runtime_error( "Method not implemented" ); }


					size_t& HttpServerImpl::max_header_count( ) { throw std::runtime_error( "Method not implemented" ); }
					size_t const & HttpServerImpl::max_header_count( ) const { throw std::runtime_error( "Method not implemented" ); }

					size_t HttpServerImpl::timeout( ) const { throw std::runtime_error( "Method not implemented" ); }

					void HttpServerImpl::on_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener ) {
						m_emitter->add_listener( "listening", listener );
					}

					void HttpServerImpl::on_next_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener ) {
						m_emitter->add_listener( "listening", listener, true );
					}

					void HttpServerImpl::on_client_connected( std::function<void( HttpConnection )> listener ) {
						auto handler = [listener]( std::shared_ptr<HttpConnection> con ) {
							listener( *con );
						};
						m_emitter->add_listener( "connection", handler );
					}

					void HttpServerImpl::on_next_client_connected( std::function<void( HttpConnection )> listener ) {
						auto handler = [listener]( std::shared_ptr<HttpConnection> con ) {
							listener( *con );
						};
						m_emitter->add_listener( "connection", handler, true );
					}
				}	// namespace impl

				HttpServer create_http_server( base::EventEmitter emitter ) {
					return HttpServer( new impl::HttpServerImpl( std::move( emitter ) ) );
				}

			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
