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
				namespace {

					void err400( std::shared_ptr<lib::net::NetSocketStream> socket_ptr ) {
						// 400 bad request
						socket_ptr->write( "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n" );
						std::stringstream stream;
						stream << "<html><body><h2>Could not parse request</h2>\r\n";
						stream << "</body></html>\r\n";
						auto body_str( stream.str( ) );
						stream.str( "" );
						stream.clear( );
						stream << "Content-Type: text/html\r\n";
						stream << "Content-Length: " << body_str.size( ) << "\r\n\r\n";
						socket_ptr->write( stream.str( ) );
						socket_ptr->end( body_str );

					}

				}
				HttpConnection::HttpConnection( std::shared_ptr<lib::net::NetSocketStream> socket_ptr ) : m_socket_ptr( std::move( socket_ptr ) ) {
					m_socket_ptr->when_next_data_recv( [&]( std::shared_ptr<base::data_t> data_buffer, bool ) mutable {
						auto req = parse_http_request( data_buffer->begin( ), data_buffer->end( ) );
						data_buffer.reset( );
						if( req ) {
							emit( "request", std::move( req ), std::make_shared<HttpServerResponse>( m_socket_ptr ) );
						} else {
							err400( m_socket_ptr );
						}
					} ).when_next_eof( [&]( ) {
						close( );
					} ).on_closed( [&]( ) {
						emit( "close" );
					} ).when_next_error( [&]( base::Error error ) {
						auto err = base::Error( "Error in connection socket" );
						err.add( "where", "HttpConnection::HttpConnection" )
							.child( std::move( error ) );
						emit( "error", std::move( err ) );						
					} );
					
					m_socket_ptr->set_read_until_values( R"((\r\n|\n){2})", true ).read_async( );
				}

				void HttpConnection::reset( ) {
					m_socket_ptr.reset( );
				}

				void HttpConnection::close( ) { 
					m_socket_ptr->close( );
				}

				std::vector<std::string> const & HttpConnection::valid_events( ) const {
					static auto const result = [&]( ) {
						auto local = std::vector < std::string > { "close" , "clientError", "requestGet", "requestPost", "requestPut", "requestHead", "requestDelete", "requestConnect", "requestOptions", "requestTrace", "request" };
						return base::impl::append_vector( local, base::EventEmitter::valid_events( ) );
					}();
					return result;
				}

				// Event callbacks

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Event emitted when an error occurs
				/// Inherited from EventEmitter
				HttpConnection& HttpConnection::when_error( std::function<void( base::Error )> listener ) {
					add_listener( "error", listener );
					return *this;
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Event emitted when an error occurs
				/// Inherited from EventEmitter
				HttpConnection& HttpConnection::when_next_error( std::function<void( base::Error )> listener ) {
					add_listener( "error", listener, true );
					return *this;
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Event emitted when the connection is closed
				HttpConnection& HttpConnection::on_closed( std::function<void( )> listener ) {
					add_listener( "close", listener, true );
					return *this;
				}

				HttpConnection& HttpConnection::when_client_error( std::function<void( base::Error )> listener ) { 
					add_listener( "clientError", listener );
					return *this;
				}

				HttpConnection& HttpConnection::when_next_client_error( std::function<void( base::Error )> listener ) { 
					add_listener( "clientError", listener, true );
					return *this;
				}

				HttpConnection& HttpConnection::when_request_made( std::function<void( std::shared_ptr<HttpClientRequest>, std::shared_ptr<HttpServerResponse> )> listener ) { 
					add_listener( "request", listener );
					return *this;
				}

				HttpConnection& HttpConnection::when_next_request_made( std::function<void( std::shared_ptr<HttpClientRequest>, std::shared_ptr<HttpServerResponse> )> listener ) {
					add_listener( "request", listener, true );
					return *this;
				}

				lib::net::NetSocketStream& HttpConnection::socket( ) {
					return *m_socket_ptr;
				}

				lib::net::NetSocketStream const & HttpConnection::socket( ) const {
					return *m_socket_ptr;
				}
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
