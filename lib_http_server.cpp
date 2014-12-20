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
				using namespace daw::nodepp;


				HttpServer::HttpServer( ) : base::EventEmitter( ), m_netserver( ) { }

				HttpServer::HttpServer( HttpServer&& other ) : base::EventEmitter( std::move( other ) ), m_netserver( std::move( other.m_netserver ) ) { }

				HttpServer& HttpServer::operator=(HttpServer && rhs) {
					if( this != &rhs ) {
						m_netserver = std::move( rhs.m_netserver );
					}
					return *this;
				}

				std::vector<std::string> const & HttpServer::valid_events( ) const {
					static auto const result = [&]( ) {
						std::vector<std::string> local{ "connection", "close", "listening" };
						return base::impl::append_vector( local, base::EventEmitter::valid_events( ) );
					}();
					return result;
				}

				void HttpServer::emit_connection( std::shared_ptr<HttpConnection> connection ) {
					emit( "connection", connection );
				}
				
				void HttpServer::emit_close( ) {
					emit( "close" );
				}

				void HttpServer::emit_listening( boost::asio::ip::tcp::endpoint endpoint ) {
					emit( "listening", std::move( endpoint ) );
				}
				
				void HttpServer::handle_connection( lib::net::NetSocketStream socket ) {
					
					std::shared_ptr<HttpConnection> connection( new HttpConnection( std::move( socket ) ) );

					connection->on_error( [&]( base::Error error ) {
						emit_error( "HttpServer::handle_connection", std::move( error ) );
					} );
										
					try {
						emit_connection( connection->get_ptr( ) );
					} catch( ... ) {
						emit_error( std::current_exception( ), "Running connection listeners", "HttpServer::handle_connection" );
					}					
				}

				void HttpServer::handle_error( base::Error error ) {
					emit_error( "HttpServer::handle_connection", std::move( error ) );
				}

				void HttpServer::listen_on( uint16_t port ) {
					m_netserver.on_connected( [&]( lib::net::NetSocketStream socket ) {
						handle_connection( std::move( socket ) );
					} );

					m_netserver.on_error( [&]( base::Error error ) {
						handle_error( std::move( error ) );
					} );

					m_netserver.on_listening( [&]( boost::asio::ip::tcp::endpoint endpoint ) {
						emit_listening( std::move( endpoint ) );
					} );
					
					m_netserver.listen( port );
				}

				void HttpServer::listen_on( uint16_t, std::string, uint16_t ) { throw std::runtime_error( "Method not implemented" ); }

				void HttpServer::listen_on( std::string ) { throw std::runtime_error( "Method not implemented" ); }

				void HttpServer::listen_on( base::ServiceHandle ) { throw std::runtime_error( "Method not implemented" ); }


				size_t& HttpServer::max_header_count( ) { throw std::runtime_error( "Method not implemented" ); }
				size_t const & HttpServer::max_header_count( ) const { throw std::runtime_error( "Method not implemented" ); }

				size_t HttpServer::timeout( ) const { throw std::runtime_error( "Method not implemented" ); }

				void HttpServer::on_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener ) {
					add_listener( "listening", listener );
				}

				void HttpServer::on_next_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener ) {
					add_listener( "listening", listener, true );
				}

				void HttpServer::on_client_connected( std::function<void( HttpConnection )> listener ) {
					auto handler = [listener]( std::shared_ptr<HttpConnection> con ) {
						listener( *con );
					};
					add_listener( "connection", handler );
				}

				void HttpServer::on_next_client_connected( std::function<void( HttpConnection )> listener ) {
					auto handler = [listener]( std::shared_ptr<HttpConnection> con ) {
						listener( *con );
					};
					add_listener( "connection", handler, true );
				}
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
