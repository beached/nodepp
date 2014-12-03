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
				using namespace daw::nodepp::base;
				using namespace boost::asio::ip;

				std::vector<std::string> const & NetServer::valid_events( ) const {
					static auto const result = [&]( ) {
						std::vector<std::string> local{ "listening", "connection", "close", "error" };
						auto parent = EventEmitter::valid_events( );
						return impl::append_vector( local, parent );
					}();
					return result;
				}

				NetServer::NetServer( ): EventEmitter{ }, m_acceptor( base::ServiceHandle::get( ) ) { }
				NetServer::~NetServer( ) { }

				NetServer::NetServer( NetServer&& other ) : EventEmitter{ std::move( other ) }, m_acceptor( std::move( other.m_acceptor ) ) { }

				NetServer& NetServer::operator=(NetServer&& rhs) {
					if( this != &rhs ) {
						m_acceptor = std::move( rhs.m_acceptor );
					}
					return *this;
				}

				namespace { 
					void emit_error( NetServer* const net_server, boost::system::error_code const & err, std::string where ) {
						auto error = base::Error( err );
						error.add( "where", where );
						base::ServiceHandle::get( ).post( [net_server, error]( ) {
							net_server->emit( "error", error );
						} );
					}
				}

				void NetServer::handle_accept( SocketHandle socket, boost::system::error_code const & err ) {
					if( !err ) {
						emit( "connection", daw::make_unique<NetSocket>( socket ) );						
						start_accept( );
					} else {
						emit_error( this, err, "NetServer::listen" );
					}
				}	

				void NetServer::start_accept( ) {
					SocketHandle socket( base::ServiceHandle::get( ) );
					auto handle = boost::bind( &NetServer::handle_accept, this, socket, boost::asio::placeholders::error );
					m_acceptor.async_accept( *socket, handle );
				}

				NetServer& NetServer::listen( uint16_t port ) {
					boost::asio::ip::tcp::resolver resolver( m_acceptor.get_io_service( ) );
					boost::asio::ip::tcp::resolver::query query( "", boost::lexical_cast<std::string>( port ) );
					boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve( query );					
					m_acceptor.open( endpoint.protocol( ) );
					m_acceptor.set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ) );
					m_acceptor.bind( endpoint );
					m_acceptor.listen( );
					start_accept( );
					return *this;
				}
				NetServer& NetServer::listen( uint16_t port, std::string hostname, uint16_t backlog ) { throw std::runtime_error( "Method not implemented" ); }
				NetServer& NetServer::listen( std::string socket_path ) { throw std::runtime_error( "Method not implemented" ); }
				NetServer& NetServer::listen( SocketHandle handle ) { throw std::runtime_error( "Method not implemented" ); }
				NetServer& NetServer::close( ) { throw std::runtime_error( "Method not implemented" ); }
				daw::nodepp::lib::net::NetAddress const & NetServer::address( ) const { throw std::runtime_error( "Method not implemented" ); }
				NetServer& NetServer::unref( ) { throw std::runtime_error( "Method not implemented" ); }
				NetServer& NetServer::ref( ) { throw std::runtime_error( "Method not implemented" ); }
				NetServer& NetServer::set_max_connections( uint16_t value ) { throw std::runtime_error( "Method not implemented" ); }
				NetServer& NetServer::get_connections( std::function<void( Error err, uint16_t count )> callback ) { throw std::runtime_error( "Method not implemented" ); }

				NetServer& NetServer::on_connection( std::function<void( std::unique_ptr<NetSocket> socket_ptr )> listener ) {
					add_listener( "connection", listener );
					return *this;
				}

				NetServer& NetServer::on_error( std::function<void( base::Error )> listener ) {
					add_listener( "error", listener );
					return *this;
				}

			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
