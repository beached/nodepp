#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <memory>
#include <utility>

#include "base_event_emitter.h"
#include "base_service_handle.h"
#include "base_types.h"
#include "lib_net_server.h"
#include "lib_net_socket_stream.h"
#include "make_unique.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				namespace impl {

					using namespace daw::nodepp;
					using namespace boost::asio::ip;


					NetServerImpl::NetServerImpl( base::EventEmitter emitter ) :
						m_acceptor( std::make_shared<boost::asio::ip::tcp::acceptor>( base::ServiceHandle::get( ) ) ),
						m_emitter( emitter ) { }

					NetServerImpl::NetServerImpl( NetServerImpl&& other ) :
						m_acceptor( std::move( other.m_acceptor ) ),
						m_emitter( std::move( other.m_emitter ) ) { }

					NetServerImpl& NetServerImpl::operator=(NetServerImpl && rhs) {
						if( this != &rhs ) {
							m_acceptor = std::move( rhs.m_acceptor );
							m_emitter = std::move( rhs.m_emitter );
						}
						return *this;
					}

// 					std::shared_ptr<NetServerImpl> NetServerImpl::get_ptr( ) {
// 						return shared_from_this( );
// 					}

					base::EventEmitter& NetServerImpl::emitter( ) {
						return m_emitter;
					}

					void NetServerImpl::listen( uint16_t port ) {
						auto endpoint = boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4( ), port );
						m_acceptor->open( endpoint.protocol( ) );
						m_acceptor->set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ) );
						m_acceptor->bind( endpoint );
						m_acceptor->listen( 511 );
						start_accept( );
						emit_listening( std::move( endpoint ) );
					}

					void NetServerImpl::listen( uint16_t, std::string hostname, uint16_t ) {
						throw std::runtime_error( "Method not implemented" );
					}

					void NetServerImpl::listen( std::string ) {
						throw std::runtime_error( "Method not implemented" );
					}

					void NetServerImpl::close( ) {
						throw std::runtime_error( "Method not implemented" );
					}

					daw::nodepp::lib::net::NetAddress const & NetServerImpl::address( ) const {
						throw std::runtime_error( "Method not implemented" );
					}

					void NetServerImpl::set_max_connections( uint16_t ) {
						throw std::runtime_error( "Method not implemented" );
					}

					void NetServerImpl::get_connections( std::function<void( base::Error err, uint16_t count )> callback ) {
						throw std::runtime_error( "Method not implemented" );
					}

					// Event callbacks				
					NetServerImpl& NetServerImpl::on_connection( std::function<void( NetSocketStream socket )> listener ) {
						emitter( )->add_listener( "connection", listener );
						return *this;
					}

					NetServerImpl& NetServerImpl::on_next_connection( std::function<void( NetSocketStream socket_ptr )> listener ) {
						emitter( )->add_listener( "connection", listener, true );
						return *this;
					}

					NetServerImpl& NetServerImpl::on_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener ) {
						emitter( )->add_listener( "listening", listener );
						return *this;
					}

					NetServerImpl& NetServerImpl::on_next_listening( std::function<void( )> listener ) {
						emitter( )->add_listener( "listening", listener, true );
						return *this;
					}

					NetServerImpl& NetServerImpl::on_closed( std::function<void( )> listener ) {
						emitter( )->add_listener( "closed", listener, true );
						return *this;
					}

					void NetServerImpl::handle_accept( std::weak_ptr<NetServerImpl> obj, NetSocketStream&& socket, boost::system::error_code const & err ) {
						auto msocket = daw::as_move_only( std::move( socket ) );
						run_if_valid( obj, "Exception while accepting connections", "NetServerImpl::handle_accept", [msocket, &err]( std::shared_ptr<NetServerImpl>& self ) mutable {
							if( !err ) {
								try {
									self->emit_connection( msocket.move_out( ) );
								} catch( ... ) {
									self->emit_error( std::current_exception( ), "Running connection listeners", "NetServerImpl::listen#emit_connection" );
								}
							} else {
								self->emit_error( err, "NetServerImpl::listen" );
							}
							self->start_accept( );
						} );
					}

					void NetServerImpl::start_accept( ) {
						auto socket_sp = create_net_socket_stream( );
						auto& boost_socket = socket_sp->socket( );
						auto socket = as_move_only( std::move( socket_sp ) );

						std::weak_ptr<NetServerImpl> obj = get_ptr( );
						auto handler = [obj, socket]( boost::system::error_code const & err ) mutable {
							handle_accept( obj, socket.move_out( ), err );
						};						
						m_acceptor->async_accept( boost_socket, handler );
					}

					void NetServerImpl::emit_connection( NetSocketStream socket ) {
						emitter( )->emit( "connection", socket );
					}

					void NetServerImpl::emit_listening( boost::asio::ip::tcp::endpoint endpoint ) {
						emitter( )->emit( "listening", std::move( endpoint ) );
					}

					void NetServerImpl::emit_closed( ) {
						emitter( )->emit( "closed" );
					}

				}	// namespace impl

				NetServer create_net_server( base::EventEmitter emitter ) {
					return NetServer( new impl::NetServerImpl( std::move( emitter ) ) );
				}

			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
