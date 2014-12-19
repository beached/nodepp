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


				HttpServer::HttpServer( ) : base::EventEmitter( ), m_netserver( ), m_connections( std::make_shared<std::list<HttpConnection>>( ) ) { }

				HttpServer::HttpServer( HttpServer&& other ) : base::EventEmitter( std::move( other ) ), m_netserver( std::move( other.m_netserver ) ), m_connections( std::move( other.m_connections ) ) { }

				HttpServer& HttpServer::operator=(HttpServer rhs) {
					m_netserver = std::move( rhs.m_netserver );
					m_connections = std::move( rhs.m_connections );
					return *this;
				}

				std::vector<std::string> const & HttpServer::valid_events( ) const {
					static auto const result = [&]( ) {
						std::vector<std::string> local{ "connection", "close", "listening" };
						return base::impl::append_vector( local, base::EventEmitter::valid_events( ) );
					}();
					return result;
				}

				namespace {
					void emit_connection( HttpServer& server, HttpConnection connection ) {
						server.emit( "connection", connection );
					}

					void emit_error( HttpServer & server, boost::system::error_code const & err, std::string where ) {
						auto error = base::Error( err );
						error.add( "where", where );
						server.emit( "error", error );
					}

					void emit_error( HttpServer & server, std::exception_ptr ex, std::string description, std::string where ) {
						auto error = base::Error( description, ex );
						error.add( "where", where );
						server.emit( "error", error );
					}
				}

				void HttpServer::handle_connection( lib::net::NetSocketStream socket ) {
					daw::algorithm::erase_remove_if( m_connections, []( std::pair<bool, HttpConnection> const & item ) {
						return item.first;
					} );

					auto connection_ptr = m_connections.emplace( m_connections.begin( ), std::make_pair<bool, HttpConnection>( false, HttpConnection( std::move( socket ) ) ) );
					connection_ptr->second.when_error( [&]( base::Error error ) {
						auto err = base::Error( "Error in connection" );
						err.add( "where", "HttpServer::handle_connection" )
							.child( std::move( error ) );
						emit( "error", std::move( err ) );
					} ).on_closed( [connection_ptr]( ) mutable {
						connection_ptr->first = true;
					} );
					try {
						emit_connection( *this, connection_ptr->second );
					} catch( ... ) {
						emit_error( *this, std::current_exception( ), "Running connection listeners", "HttpServer::handle_connection" );
					}
				}

				void HttpServer::handle_error( base::Error error ) {
					auto err = base::Error( "Child error" ).add( "where", "HttpServer::handle_error" ).child( error );
					emit( "error", err );
				}

				HttpServer& HttpServer::listen_on( uint16_t port ) {
					m_netserver.when_connected( [&]( lib::net::NetSocketStream socket ) {
						handle_connection( std::move( socket ) );
					} ).when_error( [&]( base::Error error ) {
						handle_error( std::move( error ) );
					} ).when_listening( [&]( boost::asio::ip::tcp::endpoint endpoint ) {
						emit( "listening", endpoint );
					} ).listen( port );
					return *this;
				}

				HttpServer& HttpServer::listen_on( uint16_t, std::string, uint16_t ) { throw std::runtime_error( "Method not implemented" ); }

				HttpServer& HttpServer::listen_on( std::string ) { throw std::runtime_error( "Method not implemented" ); }

				HttpServer& HttpServer::listen_on( base::ServiceHandle ) { throw std::runtime_error( "Method not implemented" ); }


				size_t& HttpServer::max_header_count( ) { throw std::runtime_error( "Method not implemented" ); }
				size_t const & HttpServer::max_header_count( ) const { throw std::runtime_error( "Method not implemented" ); }

				size_t HttpServer::timeout( ) const { throw std::runtime_error( "Method not implemented" ); }

				HttpServer& HttpServer::when_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener ) {
					add_listener( "listening", listener );
					return *this;
				}

				HttpServer& HttpServer::when_next_listening( std::function<void( boost::asio::ip::tcp::endpoint )> listener ) {
					add_listener( "listening", listener, true );
					return *this;
				}

				HttpServer& HttpServer::when_client_connected( std::function<void( HttpConnection )> listener ) {
					add_listener( "connection", listener );
					return *this;
				}

				HttpServer& HttpServer::when_next_client_connected( std::function<void( HttpConnection )> listener ) {
					add_listener( "connection", listener, true );
					return *this;
				}
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
