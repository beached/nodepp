#include <regex>
#include <boost/regex.hpp>
#include <memory>

#include "lib_http_connection.h"
#include "lib_http_request_parser.h"
#include "lib_net_socket.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				namespace {

					void err400( std::shared_ptr<lib::net::NetSocket> socket_ptr ) {
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

					void err505( std::shared_ptr<lib::net::NetSocket> socket_ptr ) {
						socket_ptr->write( "HTTP/1.1 505 HTTP Version Not Supported\r\nConnection: close\r\n" );
						std::stringstream stream;
						stream << "<html><body><h2>HTTP Version Not Supported</h2>\r\n";
						stream << "<div>This server only supports the HTTP/1.1 protocol\r\n";
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
				HttpConnection::HttpConnection( std::shared_ptr<lib::net::NetSocket> socket_ptr ): m_socket_ptr( socket_ptr ) {
					m_socket_ptr->set_read_until_values( R"((\r\n|\n){2})", true );
					m_socket_ptr->once_data( [&]( std::shared_ptr<base::data_t> data_buffer, bool ) {
						
						auto req = parse_http_request( data_buffer->begin( ), data_buffer->end( ) );
												
						if( req ) {
							if( req->request.version != "1.1" ) {
								err505( m_socket_ptr );
							}
							HttpServerResponse resp;
							
							std::string method = "Get";
							switch( req->request.method ) {
							case HttpRequestMethod::Post:
								method = "Post";
								break;
							case HttpRequestMethod::Connect:
								method = "Connect";
								break;
							case HttpRequestMethod::Delete:
								method = "Delete";
								break;
							case HttpRequestMethod::Head:
								method = "Head";
								break;
							case HttpRequestMethod::Options:
								method = "Options";
								break;
							case HttpRequestMethod::Put:
								method = "Put";
								break;
							case HttpRequestMethod::Trace:
								method = "Trace";
								break;
							}
							emit( "request", req->request.method, *req, resp );
							emit( "request" + method, *req, resp );
						} else {
							err400( m_socket_ptr );
						}

					} );
					m_socket_ptr->read_async( );
				}

				void HttpConnection::reset( ) { }

				void HttpConnection::close( ) { }

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
				HttpConnection& HttpConnection::on_error( std::function<void( base::Error )> listener ) {
					add_listener( "error", listener );
					return *this;
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Event emitted when an error occurs
				/// Inherited from EventEmitter
				HttpConnection& HttpConnection::once_error( std::function<void( base::Error )> listener ) {
					add_listener( "error", listener, true );
					return *this;
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Event emitted when the connection is closed
				HttpConnection& HttpConnection::once_close( std::function<void( )> listener ) {
					add_listener( "close", listener, true );
					return *this;
				}

				HttpConnection& HttpConnection::on_clientError( std::function<void( base::Error )> listener ) { 
					add_listener( "clientError", listener );
					return *this;
				}

				HttpConnection& HttpConnection::once_clientError( std::function<void( base::Error )> listener ) { 
					add_listener( "clientError", listener, true );
					return *this;
				}

				HttpConnection& HttpConnection::on_requestGet( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) { 
					add_listener( "requestGet", listener );
					return *this;
				}

				HttpConnection& HttpConnection::once_requestGet( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
					add_listener( "requestGet", listener, true );
					return *this;
				}

				HttpConnection& HttpConnection::on_requestPost( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
					add_listener( "requestPost", listener );
					return *this;
				}

				HttpConnection& HttpConnection::once_requestPost( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
					add_listener( "requestPost", listener, true );
					return *this;
				}

				HttpConnection& HttpConnection::on_requestPut( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
					add_listener( "requestPut", listener );
					return *this;
				}

				HttpConnection& HttpConnection::once_requestPut( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
					add_listener( "requestPut", listener, true );
					return *this;
				}

				HttpConnection& HttpConnection::on_requestHead( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
					add_listener( "requestHead", listener );
					return *this;
				}

				HttpConnection& HttpConnection::once_requestHead( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
					add_listener( "requestHead", listener, true );
					return *this;
				}

				HttpConnection& HttpConnection::on_requestDelete( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
					add_listener( "requestDelete", listener );
					return *this;
				}

				HttpConnection& HttpConnection::once_requestDelete( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
					add_listener( "requestDelete", listener, true );
					return *this;
				}

				HttpConnection& HttpConnection::on_requestConnect( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
					add_listener( "requestConnect", listener );
					return *this;
				}

				HttpConnection& HttpConnection::once_requestConnect( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
					add_listener( "requestConnect", listener, true );
					return *this;
				}

				HttpConnection& HttpConnection::on_requestOptions( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
					add_listener( "requestOptions", listener );
					return *this;
				}

				HttpConnection& HttpConnection::once_requestOptions( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
					add_listener( "requestOptions", listener, true );
					return *this;
				}

				HttpConnection& HttpConnection::on_requestTrace( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
					add_listener( "requestTrace", listener );
					return *this;
				}

				HttpConnection& HttpConnection::once_requestTrace( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
					add_listener( "requestTrace", listener, true );
					return *this;
				}

				HttpConnection& HttpConnection::on_request( std::function<void( HttpRequestMethod, HttpClientRequest, HttpServerResponse )> listener ) {
					add_listener( "request", listener );
					return *this;
				}

				HttpConnection& HttpConnection::once_request( std::function<void( HttpRequestMethod, HttpClientRequest, HttpServerResponse )> listener ) {
					add_listener( "request", listener, true );
					return *this;
				}
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
