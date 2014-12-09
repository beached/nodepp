#include <boost/shared_ptr.hpp>
#include <cinttypes>
#include <iterator>
#include <string>
#include <utility>

#include "base_event_emitter.h"
#include "base_url.h"
#include "lib_http_incoming_request.h"
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
						std::vector<std::string> local{ "request", "connection", "close", "checkContinue", "connect", "upgrade", "clientError", "listening" };
						return base::impl::append_vector( local, base::EventEmitter::valid_events( ) );
					}();
					return result;
				}

				void HttpServer::handle_connection( std::shared_ptr<lib::net::NetSocket> socket_ptr ) {
					socket_ptr->set_read_until_values( R"((\r\n|\n){2})", true )
						.on_data( [socket_ptr]( std::shared_ptr<daw::nodepp::base::data_t> data_buffer, bool ) {
						socket_ptr->set_read_mode( lib::net::NetSocket::ReadUntil::buffer_full );
						// Start connection
						
						std::string buff( data_buffer->begin( ), data_buffer->end( ) );
						std::cout << buff;
						*socket_ptr << buff;
					} ).on_end( []( ) {
						std::cout << "\n\n" << std::endl;
					} ).read_async( );
				}

				void HttpServer::handle_error( base::Error error ) {
					auto err = base::Error( "Child error" ).add( "where", "HttpServer::handle_error" ).set_child( error );
					emit( "error", err );
				}

				HttpServer& HttpServer::listen( uint16_t port ) {
					m_netserver.once_connection( [&]( std::shared_ptr<lib::net::NetSocket> socket_ptr ) {
						handle_connection( socket_ptr );
					} ).on_error( std::bind( &HttpServer::handle_error, this, std::placeholders::_1 ) )
						.listen( port );
					return *this;
				}

				HttpServer& HttpServer::listen( uint16_t, std::string, uint16_t ) { throw std::runtime_error( "Method not implemented" ); }

				HttpServer& HttpServer::listen( std::string ) { throw std::runtime_error( "Method not implemented" ); }

				HttpServer& HttpServer::listen( base::ServiceHandle ) { throw std::runtime_error( "Method not implemented" ); }


				size_t& HttpServer::max_header_count( ) { throw std::runtime_error( "Method not implemented" ); }
				size_t const & HttpServer::max_header_count( ) const { throw std::runtime_error( "Method not implemented" ); }

				size_t HttpServer::timeout( ) const { throw std::runtime_error( "Method not implemented" ); }

				HttpServer& HttpServer::on_listening( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
					add_listener( "listening", listener, false );
					return *this;
				}
				HttpServer& HttpServer::once_listening( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
					add_listener( "listening", listener, true );
					return *this;
				}

				HttpServer& create_server( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
					return HttpServer( ).on_listening( listener );
				}
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
