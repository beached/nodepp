#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <memory>
#include <utility>

#include "base_event_emitter.h"
#include "base_service_handle.h"
#include "base_types.h"
#include "lib_net_server.h"
#include "lib_net_socket.h"
#include "make_unique.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				using namespace daw::nodepp;
				using namespace boost::asio::ip;

				namespace {
					void emit_error( NetServer* const net_server, boost::system::error_code const & err, std::string where ) {
						auto error = base::Error( err );
						error.add( "where", where );
						net_server->emit( "error", error );
					}

					void emit_error( NetServer* const net_server, std::exception_ptr err, std::string where ) {
						auto error = base::Error( "Exception Caught", err );
						error.add( "where", where );
						net_server->emit( "error", error );
					}
				}	// namespace anonymous

				std::vector<std::string> const & NetServer::valid_events( ) const {
					static auto const result = [&]( ) {
						std::vector<std::string> local{ "listening", "connection", "close" };
						auto parent = EventEmitter::valid_events( );
						return base::impl::append_vector( local, parent );
					}();
					return result;
				}

				NetServer::NetServer( ): EventEmitter{ }, m_acceptor( std::make_shared<boost::asio::ip::tcp::acceptor>( base::ServiceHandle::get( ) ) ), m_current_connections( ) { }
				
				NetServer::~NetServer( ) { }

				NetServer::NetServer( NetServer&& other ) : EventEmitter{ std::move( other ) }, m_acceptor( std::move( other.m_acceptor ) ), m_current_connections( std::move( other.m_current_connections ) ) { }

				NetServer& NetServer::operator=(NetServer&& rhs) {
					if( this != &rhs ) {
						m_acceptor = std::move( rhs.m_acceptor );
						m_current_connections = std::move( rhs.m_current_connections );
					}
					return *this;
				}

				void NetServer::handle_accept( std::shared_ptr<NetSocket> socket_ptr, boost::system::error_code const & err ) {
					if( !err ) {
						try {
							emit( "connection", socket_ptr );
						} catch( ... ) {
							emit_error( this, std::current_exception( ), "NetServer::listen" );
						}
					} else {
						emit_error( this, err, "NetServer::listen" );
					}
					start_accept( );
				}	

				void NetServer::start_accept( ) {
					auto new_connection_it = m_current_connections.insert( std::end( m_current_connections ), std::make_shared<NetSocket>( base::ServiceHandle::get( ) ) );
					auto& new_connection = *new_connection_it;
					new_connection->on_end( [&, new_connection_it]( ) {
						m_current_connections.erase( new_connection_it );
					} );
					auto handle = boost::bind( &NetServer::handle_accept, this, new_connection, boost::asio::placeholders::error );
					m_acceptor->async_accept( new_connection->socket( ), handle );
				}

				NetServer& NetServer::listen( uint16_t port ) {
					auto endpoint = boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4( ), port );
					m_acceptor->open( endpoint.protocol( ) );
					m_acceptor->set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ) );
					m_acceptor->bind( endpoint );
					m_acceptor->listen( );
					start_accept( );
					emit( "listening", endpoint );
					return *this;
				}
				NetServer& NetServer::listen( uint16_t, std::string hostname, uint16_t ) { throw std::runtime_error( "Method not implemented" ); }
				NetServer& NetServer::listen( std::string ) { throw std::runtime_error( "Method not implemented" ); }
				NetServer& NetServer::listen( SocketHandle ) { throw std::runtime_error( "Method not implemented" ); }
				NetServer& NetServer::close( ) { throw std::runtime_error( "Method not implemented" ); }
				daw::nodepp::lib::net::NetAddress const & NetServer::address( ) const { throw std::runtime_error( "Method not implemented" ); }
				NetServer& NetServer::unref( ) { throw std::runtime_error( "Method not implemented" ); }
				NetServer& NetServer::ref( ) { throw std::runtime_error( "Method not implemented" ); }
				NetServer& NetServer::set_max_connections( uint16_t ) { throw std::runtime_error( "Method not implemented" ); }
				NetServer& NetServer::get_connections( std::function<void( base::Error err, uint16_t count )> callback ) { throw std::runtime_error( "Method not implemented" ); }


				// Event callbacks

				NetServer& NetServer::on_connection( std::function<void( std::shared_ptr<NetSocket> socket_ptr )> listener ) {
					add_listener( "connection", listener );
					return *this;
				}

				NetServer& NetServer::on_error( std::function<void( base::Error )> listener ) {
					add_listener( "error", listener );
					return *this;
				}

				NetServer& NetServer::on_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener ) {
					add_listener( "listening", listener );
					return *this;
				}

				NetServer& NetServer::once_error( std::function<void( base::Error )> listener ) {
					add_listener( "error", listener, true );
					return *this;
				}

				NetServer& NetServer::once_connection( std::function<void( std::shared_ptr<NetSocket> socket_ptr )> listener ) {
					add_listener( "connection", listener, true );
					return *this;
				}

				NetServer& NetServer::once_listening( std::function<void( )> listener ) {
					add_listener( "listening", listener, true );
					return *this;
				}

				NetServer& NetServer::once_close( std::function<void( )> listener ) {
					add_listener( "close", listener, true );
					return *this;
				}

			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
