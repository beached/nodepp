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
						void err400( lib::net::NetSocketStream socket ) {
							// 400 bad request
							socket.write( "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n" );
							std::stringstream stream;
							stream << "<html><body><h2>Could not parse request</h2>\r\n";
							stream << "</body></html>\r\n";
							auto body_str( stream.str( ) );
							stream.str( "" );
							stream.clear( );
							stream << "Content-Type: text/html\r\n";
							stream << "Content-Length: " << body_str.size( ) << "\r\n\r\n";
							socket.write( stream.str( ) );
							socket.end( body_str );

						}

					}	// namespace anonymous

					class HttpConnectionImpl: public base::EventEmitter {
						lib::net::NetSocketStream m_socket;

					public:
						HttpConnectionImpl( ) = delete;
						HttpConnectionImpl( lib::net::NetSocketStream socket );
						HttpConnectionImpl( HttpConnectionImpl const & ) = delete;
						HttpConnectionImpl& operator=(HttpConnectionImpl const &) = delete;
						HttpConnectionImpl( HttpConnectionImpl && ) = delete;
						HttpConnectionImpl& operator=(HttpConnectionImpl &&) = delete;
						virtual ~HttpConnectionImpl( ) = default;

						virtual std::vector<std::string> const & valid_events( ) const override;

						virtual void when_client_error( std::function<void( base::Error )> listener );
						virtual void when_next_client_error( std::function<void( base::Error )> listener );

						virtual void when_request_made( std::function<void( std::shared_ptr<HttpClientRequest>, HttpServerResponse )> listener );
						virtual void when_next_request_made( std::function<void( std::shared_ptr<HttpClientRequest>, HttpServerResponse )> listener );

						virtual void when_closed( std::function<void( )> listener );	// Only once as it is called on the way out				
						void close( );

						lib::net::NetSocketStream& socket( );
						lib::net::NetSocketStream const & socket( ) const;						

					protected:
						virtual void emit_close( );
						virtual void emit_client_error( base::Error error );
						virtual void emit_request( std::shared_ptr<HttpClientRequest> request, HttpServerResponse response );
					};	// class HttpConnectionImpl

					HttpConnectionImpl::HttpConnectionImpl( lib::net::NetSocketStream socket ) : m_socket( std::move( socket ) ) {
						m_socket.when_next_data_recv( [&]( std::shared_ptr<base::data_t> data_buffer, bool ) mutable {
							auto req = parse_http_request( data_buffer->begin( ), data_buffer->end( ) );
							data_buffer.reset( );
							if( req ) {
								emit_request( std::move( req ), HttpServerResponse( std::move( m_socket ) ) );
							} else {
								err400( std::move( m_socket ) );
							}
						} );

						m_socket.when_closed( [&]( ) {
							emit_close( );
						} );

						m_socket.when_error( [&]( base::Error error ) {
							emit_error( "HttpConnectionImpl::HttpConnectionImpl", std::move( error ) );
						} );

						m_socket.set_read_until_values( R"((\r\n|\n){2})", true );

						m_socket.read_async( );
					}

					void HttpConnectionImpl::close( ) {
						m_socket.close( );
					}

					std::vector<std::string> const & HttpConnectionImpl::valid_events( ) const {
						static auto const result = [&]( ) {
							auto local = std::vector < std::string > { "close", "clientError", "request" };
							return base::impl::append_vector( local, base::EventEmitter::valid_events( ) );
						}();
						return result;
					}

					void HttpConnectionImpl::emit_close( ) {
						emit( "close" );
					}

					void HttpConnectionImpl::emit_client_error( base::Error error ) {
						emit( "clientError", std::move( error ) );
					}

					void HttpConnectionImpl::emit_request( std::shared_ptr<HttpClientRequest> request, HttpServerResponse response ) {
						emit( "request", std::move( request ), std::move( response ) );
					}


					// Event callbacks

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the connection is closed
					void HttpConnectionImpl::when_closed( std::function<void( )> listener ) {
						add_listener( "close", listener, true );
					}

					void HttpConnectionImpl::when_client_error( std::function<void( base::Error )> listener ) {
						add_listener( "clientError", listener );
					}

					void HttpConnectionImpl::when_next_client_error( std::function<void( base::Error )> listener ) {
						add_listener( "clientError", listener, true );
					}

					void HttpConnectionImpl::when_request_made( std::function<void( std::shared_ptr<HttpClientRequest>, HttpServerResponse )> listener ) {
						add_listener( "request", listener );
					}

					void HttpConnectionImpl::when_next_request_made( std::function<void( std::shared_ptr<HttpClientRequest>, HttpServerResponse )> listener ) {
						add_listener( "request", listener, true );
					}

					lib::net::NetSocketStream& HttpConnectionImpl::socket( ) {
						return m_socket;
					}

					lib::net::NetSocketStream const & HttpConnectionImpl::socket( ) const {
						return m_socket;
					}

				}	// namespace impl


				HttpConnection::HttpConnection( lib::net::NetSocketStream socket ) : m_impl( std::make_shared<impl::HttpConnectionImpl>( std::move( socket ) ) ) { }
				
				HttpConnection::HttpConnection( HttpConnection && other ): m_impl( std::move( other.m_impl ) ) { }
				
				HttpConnection& HttpConnection::operator=(HttpConnection rhs) {
					m_impl = std::move( rhs.m_impl );
					return *this;
				}

				std::vector<std::string> const & HttpConnection::valid_events( ) const {
					return m_impl->valid_events( );
				}

				// Event callbacks
				
				void HttpConnection::when_listener_added( std::function<void( std::string, base::Callback )> listener ) {
					m_impl->when_listener_added( listener );
				}

				void HttpConnection::when_listener_removed( std::function<void( std::string, base::Callback )> listener ) {
					m_impl->when_listener_removed( listener );
				}

				void HttpConnection::when_error( std::function<void( base::Error )> listener ) {
					m_impl->when_error( listener );
				}

				void HttpConnection::when_next_listener_added( std::function<void( std::string, base::Callback )> listener ) {
					m_impl->when_next_listener_added( listener );
				}

				void HttpConnection::when_next_listener_removed( std::function<void( std::string, base::Callback )> listener ) {
					m_impl->when_next_listener_removed( listener );
				}

				void HttpConnection::when_next_error( std::function<void( base::Error )> listener ) {
					m_impl->when_next_error( listener );
				}

				void HttpConnection::when_client_error( std::function<void( base::Error )> listener ) {
					m_impl->when_client_error( listener );
				}

				void HttpConnection::when_next_client_error( std::function<void( base::Error )> listener ) {
					m_impl->when_next_client_error( listener );
				}

				void HttpConnection::when_request_made( std::function<void( std::shared_ptr<HttpClientRequest>, HttpServerResponse )> listener ) {
					m_impl->when_request_made( listener );
				}

				void HttpConnection::when_next_request_made( std::function<void( std::shared_ptr<HttpClientRequest>, HttpServerResponse )> listener ) {
					m_impl->when_next_request_made( listener );
				}

				void HttpConnection::when_closed( std::function<void( )> listener ) {	// Only once as it is called on the way out
					auto handler = [&]( ) {
						auto self = get_ptr( );
						listener( );
					};
					m_impl->when_closed( handler );
				}

				void HttpConnection::close( ) {
					m_impl->close( );
				}

				std::shared_ptr<HttpConnection> HttpConnection::get_ptr( ) {
					return shared_from_this( );
				}



				lib::net::NetSocketStream& HttpConnection::socket( ) { return m_impl->socket( ); }
				lib::net::NetSocketStream const & HttpConnection::socket( ) const { return m_impl->socket( ); }

			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
