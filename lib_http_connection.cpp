#include <regex>
#include <boost/regex.hpp>
#include <memory>

#include "lib_http_connection.h"
#include "lib_http_request.h"
#include "lib_net_socket_stream.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				namespace impl {
					namespace {
						void err400( lib::net::NetSocketStream& socket ) {
							// 400 bad request
							socket->write_async( "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n" );
							std::stringstream stream;
							stream << "<html><body><h2>Could not parse request</h2>\r\n";
							stream << "</body></html>\r\n";
							auto body_str( stream.str( ) );
							stream.str( "" );
							stream.clear( );
							stream << "Content-Type: text/html\r\n";
							stream << "Content-Length: " << body_str.size( ) << "\r\n\r\n";
							socket->write_async( stream.str( ) );
							socket->end( body_str );

						}

					}	// namespace anonymous


					HttpConnectionImpl::HttpConnectionImpl( lib::net::NetSocketStream socket, base::EventEmitter emitter ) :
						m_socket( std::move( socket ) ),
						m_emitter( std::move( emitter) ) {
					}

					void HttpConnectionImpl::start( ) {
						std::weak_ptr<HttpConnectionImpl> obj( get_ptr( ) );

						m_socket->on_next_data_received( [obj] ( std::shared_ptr<base::data_t> data_buffer, bool ) mutable {
							run_if_valid( obj, "Exception in processing received data", "HttpConnectionImpl::start#on_next_data_received", [&]( std::shared_ptr<HttpConnectionImpl> self ) {
								auto request = parse_http_request( data_buffer->begin( ), data_buffer->end( ) );
								data_buffer.reset( );
								if( request ) {
									auto response = create_http_server_response( self->m_socket->get_weak_ptr( ) );
									response->start( );
									self->emit_request_made( request, response );
								} else {
									err400( self->m_socket );
								}
							} );
						} ).delegate_to( "closed", obj, "closed" )
							.delegate_error_to( obj, "HttpConnectionImpl::start" )
							.set_read_until_values( R"((\r\n|\n){2})", true );

						m_socket->read_async( );
					}
 
					HttpConnectionImpl::~HttpConnectionImpl( ) { }

					base::EventEmitter& HttpConnectionImpl::emitter( ) {
						return m_emitter;
					}

					void HttpConnectionImpl::close( ) {
						m_socket->close( );
					}

					void HttpConnectionImpl::emit_closed( ) {
						emitter( )->emit( "closed" );
					}

					void HttpConnectionImpl::emit_client_error( base::Error error ) {
						emitter( )->emit( "client_error", error );
					}

					void HttpConnectionImpl::emit_request_made( std::shared_ptr<HttpClientRequest> request, HttpServerResponse response ) {
						emitter( )->emit( "request_made", request, response );
					}


					// Event callbacks

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the connection is closed
					HttpConnectionImpl& HttpConnectionImpl::on_closed( std::function<void( )> listener ) {
						emitter( )->add_listener( "closed", listener, true );
						return *this;
					}

					HttpConnectionImpl& HttpConnectionImpl::on_client_error( std::function<void( base::Error )> listener ) {
						emitter( )->add_listener( "client_error", listener );
						return *this;
					}

					HttpConnectionImpl& HttpConnectionImpl::on_next_client_error( std::function<void( base::Error )> listener ) {
						emitter( )->add_listener( "client_error", listener, true );
						return *this;
					}

					HttpConnectionImpl& HttpConnectionImpl::on_request_made( std::function<void( std::shared_ptr<HttpClientRequest>, HttpServerResponse )> listener ) {
						emitter( )->add_listener( "request_made", listener );
						return *this;
					}

					HttpConnectionImpl& HttpConnectionImpl::on_next_request_made( std::function<void( std::shared_ptr<HttpClientRequest>, HttpServerResponse )> listener ) {
						emitter( )->add_listener( "request_made", listener, true );
						return *this;
					}

					lib::net::NetSocketStream HttpConnectionImpl::socket( ) {
						return m_socket;
					}

				}	// namespace impl

				HttpConnection create_http_connection( lib::net::NetSocketStream socket, base::EventEmitter emitter ) {
					return HttpConnection( new impl::HttpConnectionImpl( std::move( socket ), std::move( emitter ) ) );
				}

			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
