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
				using namespace daw::nodepp;
				using namespace boost::asio::ip;


				NetServer::NetServer( std::shared_ptr<base::EventEmitter> emitter ) :
					base::StandardEvents<NetServer>( emitter ),
					m_acceptor( std::make_shared<boost::asio::ip::tcp::acceptor>( base::ServiceHandle::get( ) ) ),
					m_emitter( emitter ) { }			

				NetServer::NetServer( NetServer&& other ) : 
					base::StandardEvents<NetServer>( std::move( other ) ),
					m_acceptor( std::move( other.m_acceptor ) ),
					m_emitter(std::move( other.m_emitter ) ) { }

				NetServer& NetServer::operator=(NetServer && rhs) {
					if( this != &rhs ) {
						m_acceptor = std::move( rhs.m_acceptor );
						m_emitter = std::move( rhs.m_emitter );
					}
					return *this;
				}

				std::shared_ptr<NetServer> NetServer::get_ptr( ) {
					return shared_from_this( );
				}

				void NetServer::listen( uint16_t port ) {
					auto endpoint = boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4( ), port );
					m_acceptor->open( endpoint.protocol( ) );
					m_acceptor->set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ) );
					m_acceptor->bind( endpoint );
					m_acceptor->listen( );
					start_accept( );
					emit_listening( std::move( endpoint ) );
				}

				void NetServer::listen( uint16_t, std::string hostname, uint16_t ) {
					throw std::runtime_error( "Method not implemented" ); 
				}
				
				void NetServer::listen( std::string ) { 
					throw std::runtime_error( "Method not implemented" ); 
				}
				
				void NetServer::close( ) { 
					throw std::runtime_error( "Method not implemented" ); 
				}

				daw::nodepp::lib::net::NetAddress const & NetServer::address( ) const { 
					throw std::runtime_error( "Method not implemented" ); 
				}

				void NetServer::set_max_connections( uint16_t ) { 
					throw std::runtime_error( "Method not implemented" ); 
				}
				
				void NetServer::get_connections( std::function<void( base::Error err, uint16_t count )> callback ) { 
					throw std::runtime_error( "Method not implemented" ); 
				}

				// Event callbacks				
				void NetServer::on_connection( std::function<void( SharedNetSocketStream socket )> listener ) {
					m_emitter->add_listener( "connection", listener );
				}

				void NetServer::on_next_connection( std::function<void( SharedNetSocketStream socket_ptr )> listener ) {
					m_emitter->add_listener( "connection", listener, true );
				}

				void NetServer::on_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener ) {
					m_emitter->add_listener( "listening", listener );
				}

				void NetServer::on_next_listening( std::function<void( )> listener ) {
					m_emitter->add_listener( "listening", listener, true );
				}

				void NetServer::on_closed( std::function<void( )> listener ) {
					m_emitter->add_listener( "close", listener, true );
				}

				void NetServer::handle_accept( std::weak_ptr<NetServer> obj, std::shared_ptr<NetSocketStream> socket, boost::system::error_code const & err ) {
					if( !obj.expired( ) ) {
						auto server = obj.lock( );
						if( !err ) {
							try {
								server->emit_connection( socket );
							} catch( ... ) {
								server->emit_error( std::current_exception( ), "Running connection listeners", "NetServer::listen#emit_connection" );
							}
						} else {
							server->emit_error( err, "NetServer::listen" );
						}
						server->start_accept( );
					}
				}

				void NetServer::start_accept( ) {
					auto socket = create_shared_socket( );

					auto handler = [&, socket]( boost::system::error_code const & err ) mutable {
						handle_accept( shared_from_this( ), std::move( socket ), err );
					};

					m_acceptor->async_accept( socket->socket( ), handler );
				}

				void NetServer::emit_connection( SharedNetSocketStream socket ) {
					m_emitter->emit( "connection", std::move( socket ) );
				}

				void NetServer::emit_listening( boost::asio::ip::tcp::endpoint endpoint ) {
					m_emitter->emit( "listening", std::move( endpoint ) );
				}

				void NetServer::emit_closed( ) {
					m_emitter->emit( "closed" );
				}

			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
