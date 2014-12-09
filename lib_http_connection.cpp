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
					bool set_method( std::string::const_iterator first, std::string::const_iterator last, HttpRequestMethod& method );
					bool set_url( std::string::const_iterator first, std::string::const_iterator last, base::Url & url );
					bool set_version( std::string::const_iterator first, std::string::const_iterator last, HttpVersion & version );
					class RequestLine;
				}	// namespace anonymous


				HttpConnection::HttpConnection( std::shared_ptr<lib::net::NetSocket> socket_ptr ) :m_socket_ptr( socket_ptr ), m_state( HttpConnectionState::Request ) {

				}

				void HttpConnection::reset( ) { }

				void HttpConnection::close( ) { }

				std::vector<std::string> const & HttpConnection::valid_events( ) const {
					static auto const result = [&]( ) {
						auto local = std::vector < std::string > { "close" };
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

				namespace {
					bool set_method( std::string::const_iterator first, std::string::const_iterator last, HttpRequestMethod& method ) {
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

					bool set_url( std::string::const_iterator first, std::string::const_iterator last, base::Url & url ) {
						try {
							url = std::string( first, last );
						} catch( std::exception const & ) {
							return false;
						}
						return true;
					}

					bool set_version( std::string::const_iterator first, std::string::const_iterator last, HttpVersion & version ) {
						try {
							auto slash_pos = daw::find_all_where( first, last, []( std::string::const_iterator::value_type value ) -> bool {
								return  '/' == value;
							} );
							if( slash_pos.size( ) != 1 ) {
								return false;
							} else if( !daw::equal_nc( first, slash_pos[0], "HTTP" ) ) {
								return false;
							}
							version = std::string( slash_pos[0] + 1, last );
						} catch( std::exception const & ) {
							return false;
						}
						return true;
					}

					class RequestLine {
						HttpRequestMethod m_method;
						base::Url m_url;
						HttpVersion m_version;
						bool m_is_valid;

						void build_request_line( std::string::const_iterator first, std::string::const_iterator last ) {
							m_is_valid = true;
							auto ws = daw::find_all_where( first, last, is_space );
							if( 2 != ws.size( ) ) {
								m_is_valid = false;
								return;
							}
							m_is_valid &= set_method( first, ws[0], m_method );
							m_is_valid &= set_url( ws[0] + 1, ws[1], m_url );
							m_is_valid &= set_version( ws[1] + 1, last, m_version );
						}

					public:
						RequestLine( std::string::const_iterator first, std::string::const_iterator last ) :m_method( HttpRequestMethod::Unknown ), m_url( ), m_version( ), m_is_valid( true ) {
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

			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
