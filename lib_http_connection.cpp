#include <regex>
#include <boost/regex.hpp>
#include <memory>

#include "lib_http_connection.h"
#include "lib_http_incoming_request.h"
#include "lib_net_socket.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				namespace {

					template<typename Iterator>
					bool set_method( Iterator first, Iterator last, HttpRequestMethod& method ) {
						// REDO better, works but don't like
						if( daw::equal_nc( first, last, "GET" ) ) {
							method = HttpRequestMethod::Get;
						} else if( daw::equal_nc( first, last, "POST" ) ) {
							method = HttpRequestMethod::Post;
						} else if( daw::equal_nc( first, last, "PUT" ) ) {
							method = HttpRequestMethod::Put;
						} else if( daw::equal_nc( first, last, "HEAD" ) ) {
							method = HttpRequestMethod::Head;
						} else if( daw::equal_nc( first, last, "DELETE" ) ) {
							method = HttpRequestMethod::Delete;
						} else if( daw::equal_nc( first, last, "CONNECT" ) ) {
							method = HttpRequestMethod::Connect;
						} else if( daw::equal_nc( first, last, "OPTIONS" ) ) {
							method = HttpRequestMethod::Options;
						} else if( daw::equal_nc( first, last, "TRACE" ) ) {
							method = HttpRequestMethod::Trace;
						} else {
							method = HttpRequestMethod::Unknown;
						}
						return method != HttpRequestMethod::Unknown;
					}

					template<typename Iterator>
					bool set_url( Iterator first, Iterator last, base::Url & url ) {
						try {
							url = base::Url( "hostname", "80", std::string( first, last ) );
						} catch( std::exception const & ) {
							return false;
						}
						return true;
					}

					template<typename Iterator>	 
					bool set_version( Iterator first, Iterator last, HttpVersion & version ) {
						assert( 3 == std::distance( first, last ) );
						try {
							uint8_t major = *first - '0';
							uint8_t minor = *(first + 2) - '0';
							version = HttpVersion( major, minor );
						} catch( std::exception const & ) {
							version = HttpVersion( );
							return false;
						}
						return true;
					}

					class RequestLine {
						HttpRequestMethod m_method;
						base::Url m_url;
						HttpVersion m_version;
						bool m_is_valid;

						template<typename Iterator>
						void build_request_line( Iterator first, Iterator last ) {
							// Looks for (1 or more uppercase letters e.g. GET)whitespace(/some/path/to/something?query=value)whitespaceHTTP/(version major.version minor)
							static boost::regex const request_regex( R"(^([A-Z]{1,})\s{1,}(\/.{0,})\s{1,}HTTP\/(\d{1}\.\d{1})$)" );	// TODO, check how lenient I need to be, do I need to accept escaped characters e.g. %20 
							boost::match_results<Iterator> request_parts;
							if( boost::regex_search( first, last, request_parts, request_regex ) ) {
								m_is_valid &= set_method( request_parts[1].first, request_parts[1].second, m_method );
								m_is_valid &= set_url( request_parts[2].first, request_parts[2].second, m_url );
								m_is_valid &= set_version( request_parts[3].first, request_parts[3].second, m_version );
							}							
						}

					public:
						template<typename Iterator>
						RequestLine( Iterator first, Iterator last ) :m_method( HttpRequestMethod::Unknown ), m_url( ), m_version( ), m_is_valid( true ) {
							build_request_line( first, last );
						}

						RequestLine( std::string const & line ) :m_method( HttpRequestMethod::Unknown ), m_url( ), m_version( ), m_is_valid( true ) {
							build_request_line( std::begin( line ), std::end( line ) );
						}

						bool is_valid( ) const { return m_is_valid; }

						HttpRequestMethod const & method( ) const { return m_method; }

						base::Url const & url( ) const { return m_url; }

						HttpVersion const & version( ) const { return m_version; }
					};	// class RequestLine
				}	// namespace anonymous

				template<typename Iterator>
				Iterator find_end_of_line( Iterator first, Iterator last ) {
					auto it = std::find( first, last, '\n' );
					if( it != last ) {
						--it;
						if( *it != '\r' ) {
							++it;
						}
					}
					return it;
				}

				template<typename Iterator>
				Iterator move_to_beginning_of_next_line( Iterator first, Iterator last ) {
					while( first != last && *first != '\n' ) {
						++first;
					}
					if( first != last ) {
						++first;
					}
					return first;
				}

				void err404( std::shared_ptr<lib::net::NetSocket> socket_ptr, RequestLine const & req ) {
					// 400 bad request
					socket_ptr->write( "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n" );
					std::stringstream stream;
					stream << "<html><body><h2>Could not parse request</h2>\r\n";
					if( req.method( ) == HttpRequestMethod::Unknown ) {
						stream << "<div>Could not parse request method</div>\r\n";
					}
					if( !req.url( ).is_valid( ) ) {
						stream << "<div>Could not parse request URI</div>\r\n";
					}
					if( !req.version( ).is_valid( ) ) {
						stream << "<div>Could not parse version</div>\r\n";
					}
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

				HttpConnection::HttpConnection( std::shared_ptr<lib::net::NetSocket> socket_ptr ) :m_socket_ptr( socket_ptr ), m_state( HttpConnectionState::Request ) {
					socket_ptr->set_read_until_values( R"((\r\n|\n){1})", true );
					socket_ptr->once_data( [&, socket_ptr]( std::shared_ptr<base::data_t> data_buffer, bool is_eof ) mutable {
						auto req = RequestLine( data_buffer->cbegin( ), data_buffer->cend( ) );
						if( req.is_valid( ) ) {
							if( req.version( ).major( ) != 1 || req.version( ).minor( ) != 1 ) {
								err505( socket_ptr );
							}
							HttpClientRequest client_request;
							HttpServerResponse resp;
							assert( req.method( ) != HttpRequestMethod::Unknown );
							std::string method = "Get";
							switch( req.method() ) {
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
							emit( "request", req.method(), client_request, resp );
							emit( "request" + method, client_request, resp );
						} else {
							err404( socket_ptr, req );
						}

					} );
					socket_ptr->read_async( );
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
