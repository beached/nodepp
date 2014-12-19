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

				std::vector<std::string> const & NetServer::valid_events( ) const {
					static auto const result = [&]( ) {
						std::vector<std::string> local{ "listening", "connection", "close" };
						auto parent = EventEmitter::valid_events( );
						return base::impl::append_vector( local, parent );
					}();
					return result;
				}

				void NetServer::emit_listening( boost::asio::ip::tcp::endpoint endpoint ) {
					emit( "listening", std::move( endpoint ) );
				}

				void NetServer::emit_connection( NetSocketStream socket ) {
					emit( "connection", std::move( socket ) );
				}
				
				void NetServer::emit_close( ) {
					emit( "close" );
				}

				NetServer::NetServer( ): EventEmitter( ), m_acceptor( std::make_shared<boost::asio::ip::tcp::acceptor>( base::ServiceHandle::get( ) ) ) { }			

				NetServer::NetServer( NetServer&& other ) : EventEmitter( std::move( other ) ), m_acceptor( std::move( other.m_acceptor ) ) { }

				NetServer& NetServer::operator=(NetServer rhs) {
					m_acceptor = std::move( rhs.m_acceptor );
					return *this;
				}

				void NetServer::handle_accept( NetSocketStream socket, boost::system::error_code const & err ) {
					if( !err ) {
						try {
							emit_connection( std::move( socket ) );
						} catch( ... ) {
							emit_error( std::current_exception( ) , "Running connection listeners", "NetServer::listen#emit_connection" );
						}
					} else {
						emit_error( err, "NetServer::listen" );
					}
					start_accept( );
				}	

				void NetServer::start_accept( ) {
					auto socket = NetSocketStream( base::ServiceHandle::get( ) );
					
					auto handler = [&,socket]( boost::system::error_code const & err ) mutable {
						handle_accept( std::move( socket ), err );
					};

					m_acceptor->async_accept( socket.socket( ), handler );
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
				void NetServer::listen( uint16_t, std::string hostname, uint16_t ) { throw std::runtime_error( "Method not implemented" ); }
				void NetServer::listen( std::string ) { throw std::runtime_error( "Method not implemented" ); }
				void NetServer::close( ) { throw std::runtime_error( "Method not implemented" ); }
				daw::nodepp::lib::net::NetAddress const & NetServer::address( ) const { throw std::runtime_error( "Method not implemented" ); }
				void NetServer::unref( ) { throw std::runtime_error( "Method not implemented" ); }
				void NetServer::ref( ) { throw std::runtime_error( "Method not implemented" ); }
				void NetServer::set_max_connections( uint16_t ) { throw std::runtime_error( "Method not implemented" ); }
				void NetServer::get_connections( std::function<void( base::Error err, uint16_t count )> callback ) { throw std::runtime_error( "Method not implemented" ); }


				// Event callbacks
				
				void NetServer::when_connected( std::function<void( NetSocketStream socket )> listener ) {
					add_listener( "connection", listener );
				}

				void NetServer::when_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener ) {
					add_listener( "listening", listener );
				}

				void NetServer::when_next_connection( std::function<void( NetSocketStream socket_ptr )> listener ) {
					add_listener( "connection", listener, true );
				}

				void NetServer::when_next_listening( std::function<void( )> listener ) {
					add_listener( "listening", listener, true );
				}

				void NetServer::when_closed( std::function<void( )> listener ) {
					add_listener( "close", listener, true );
				}

			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
