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
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;


				HttpServer::HttpServer( ) : base::EventEmitter( ), m_netserver( ) { }

				HttpServer::HttpServer( HttpServer&& other ) : base::EventEmitter( std::move( other ) ), m_netserver( std::move( other.m_netserver ) ) { }

				HttpServer& HttpServer::operator=(HttpServer&& rhs) {
					if( this != &rhs ) {
						m_netserver = std::move( rhs.m_netserver );
					}
					return *this;
				}

				HttpServer::~HttpServer( ) { }

				std::vector<std::string> const & HttpServer::valid_events( ) const {
					static auto const result = [&]( ) {
						std::vector<std::string> local{ "connection", "close", "listening" };
						return base::impl::append_vector( local, base::EventEmitter::valid_events( ) );
					}();
					return result;
				}

				namespace {
					void emit_connection( HttpServer& server, std::shared_ptr<HttpConnection> connection ) {
						server.emit( "connection", connection );
					}
				}

				void HttpServer::handle_connection( std::shared_ptr<lib::net::NetSocketStream> socket_ptr ) {
					auto con = std::make_shared<HttpConnection>( std::move( socket_ptr ) );
					
					con->once_close( [con]( ) {
						base::ServiceHandle::get( ).post( [con]( ) {
							con->remove_all_listeners( );
							con->socket( ).remove_all_listeners( );
							con->reset( );
						} );
					} ).on_error( [&]( base::Error error ) {
						auto err = base::Error( "Error in connection" );
						err.add( "where", "HttpServer::handle_connection" )
							.child( std::move( error ) );
						emit( "error", std::move( err ) );
					} );

					emit_connection( *this, std::move( con ) );
				}

				void HttpServer::handle_error( base::Error error ) {
					auto err = base::Error( "Child error" ).add( "where", "HttpServer::handle_error" ).child( error );
					emit( "error", err );
				}

				HttpServer& HttpServer::listen( uint16_t port ) {
					m_netserver.on_connection( [&]( std::shared_ptr<lib::net::NetSocketStream> socket_ptr ) {
							handle_connection( std::move( socket_ptr ) );
						} ).on_error( std::bind( &HttpServer::handle_error, this, std::placeholders::_1 ) )
						.on_listening( [&]( boost::asio::ip::tcp::endpoint endpoint ) {
							emit( "listening", endpoint );
						} ).listen( port );
					return *this;
				}

				HttpServer& HttpServer::listen( uint16_t, std::string, uint16_t ) { throw std::runtime_error( "Method not implemented" ); }

				HttpServer& HttpServer::listen( std::string ) { throw std::runtime_error( "Method not implemented" ); }

				HttpServer& HttpServer::listen( base::ServiceHandle ) { throw std::runtime_error( "Method not implemented" ); }


				size_t& HttpServer::max_header_count( ) { throw std::runtime_error( "Method not implemented" ); }
				size_t const & HttpServer::max_header_count( ) const { throw std::runtime_error( "Method not implemented" ); }

				size_t HttpServer::timeout( ) const { throw std::runtime_error( "Method not implemented" ); }

				HttpServer& HttpServer::on_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener ) {
					add_listener( "listening", listener );
					return *this;
				}

				HttpServer& HttpServer::once_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener ) {
					add_listener( "listening", listener, true );
					return *this;
				}

				HttpServer& HttpServer::on_connection( std::function<void( std::shared_ptr<HttpConnection> )> listener ) {
					add_listener( "connection", listener );
					return *this;
				}

				HttpServer& HttpServer::once_connection( std::function<void( std::shared_ptr<HttpConnection> )> listener ) {
					add_listener( "connection", listener, true );
					return *this;
				}
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
