#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cstdint>
#include <string>

#include "base_enoding.h"
#include "base_stream.h"
#include "lib_http.h"
#include "lib_http_headers.h"
#include "lib_http_server_response.h"
#include "range_algorithm.h"
#include "string.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				class HttpServerResponseImpl: public base::stream::StreamWritable {
					HttpVersion m_version;
					HttpHeaders m_headers;
					base::data_t m_body;
					bool m_status_sent;
					bool m_headers_sent;
					bool m_body_sent;
					lib::net::NetSocketStream m_socket;
				public:
					HttpServerResponseImpl( lib::net::NetSocketStream socket  );
					HttpServerResponseImpl( HttpServerResponseImpl const & ) = delete;
					~HttpServerResponseImpl( ) = default;
					HttpServerResponseImpl& operator=(HttpServerResponseImpl const &) = delete;
					HttpServerResponseImpl( HttpServerResponseImpl&& other ) = delete;
					HttpServerResponseImpl& operator=(HttpServerResponseImpl && rhs) = delete;

					virtual HttpServerResponseImpl& write( base::data_t const & data ) override;
					virtual HttpServerResponseImpl& write( std::string const & data, base::Encoding const & encoding = base::Encoding( ) ) override;
					virtual void end( ) override;
					virtual void end( base::data_t const & data ) override;
					virtual void end( std::string const & data, base::Encoding const & encoding = base::Encoding( ) ) override;

					void close( );

					HttpHeaders& headers( );
					HttpHeaders const & headers( ) const;

					void send_status( uint16_t status_code = 200 );
					void send_headers( );
					void send_body( );
					void clear_body( );
					bool send( );
					void reset( );
					bool is_open( );
					bool is_closed( ) const;
					bool can_write( ) const;

					HttpServerResponseImpl& add_header( std::string header_name, std::string header_value );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when a write is completed
					/// Inherited from StreamWritable
					virtual HttpServerResponseImpl& when_a_write_completes( std::function<void( )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when end( ... ) has been called and all data
					/// has been flushed
					/// Inherited from StreamWritable
					virtual HttpServerResponseImpl& when_all_writes_complete( std::function<void( )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when end( ... ) has been called and all data
					/// has been flushed
					/// Inherited from StreamWritable
					virtual HttpServerResponseImpl& when_next_all_writes_complete( std::function<void( )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the next write is completed
					/// Inherited from StreamWritable
					virtual HttpServerResponseImpl& when_next_write_completes( std::function<void( )> listener ) override;

				};	// struct HttpServerResponse						

				HttpServerResponseImpl::HttpServerResponseImpl( lib::net::NetSocketStream socket ) :
					m_version( 1, 1 ),
					m_headers( ),
					m_body( ),
					m_status_sent( false ),
					m_headers_sent( false ),
					m_body_sent( false ),
					m_socket( std::move( socket ) ) {

					m_socket.when_a_write_completes( [&]( ) {
						emit( "drain" );
					} ).when_all_writes_complete( [&]( ) {
						emit( "finish" );
					} );
				}

				HttpServerResponseImpl& HttpServerResponseImpl::write( base::data_t const & data ) {
					m_body.insert( std::end( m_body ), std::begin( data ), std::end( data ) );
					return *this;
				}

				HttpServerResponseImpl& HttpServerResponseImpl::write( std::string const & data, base::Encoding const & ) {
					m_body.insert( std::end( m_body ), std::begin( data ), std::end( data ) );
					return *this;
				}

				void HttpServerResponseImpl::clear_body( ) {
					m_body.clear( );
				}

				HttpHeaders& HttpServerResponseImpl::headers( ) {
					return m_headers;
				}

				HttpHeaders const & HttpServerResponseImpl::headers( ) const {
					return m_headers;
				}

				void HttpServerResponseImpl::send_status( uint16_t status_code ) {
					auto status = HttpStatusCodes( status_code );
					std::string msg = "HTTP/" + m_version.to_string( ) + " " + std::to_string( status.first ) + " " + status.second + "\r\n";
					//auto msg = daw::string::string_format( "HTTP/{0} {1} {2}\r\n", m_version.to_string( ), status.first, status.second );
					m_socket.write( msg ); // TODO make faster
					m_status_sent = true;
				}

				namespace {
					std::string gmt_timestamp( ) {
						auto now = time( 0 );
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif
						auto ptm = gmtime( &now );

						char buf[80];
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif
						strftime( buf, sizeof( buf ), "%a, %d %b %Y %H:%M:%S GMT", ptm );

						return buf;
					}
				}

				void HttpServerResponseImpl::send_headers( ) {
					auto& dte = m_headers["Date"];
					if( dte.empty( ) ) {
						dte = gmt_timestamp( );
					}
					m_socket.write( m_headers.to_string( ) );
					m_headers_sent = true;
				}
				void HttpServerResponseImpl::send_body( ) {
					HttpHeader content_header( "Content-Length", boost::lexical_cast<std::string>(m_body.size( )) );
					m_socket.write( content_header.to_string( ) );
					m_socket.write( "\r\n\r\n" );
					m_socket.write( m_body );
					m_body_sent = true;
				}

				bool HttpServerResponseImpl::send( ) {
					bool result = false;
					if( !m_status_sent ) {
						result = true;
						send_status( );
					}
					if( !m_headers_sent ) {
						result = true;
						send_headers( );
					}
					if( !m_body_sent ) {
						result = true;
						send_body( );
					}
					return result;
				}

				void HttpServerResponseImpl::end( ) {
					send( );
					m_socket.end( );
				}

				void HttpServerResponseImpl::end( base::data_t const & data ) {
					write( data );
					end( );
				}

				void HttpServerResponseImpl::end( std::string const & data, base::Encoding const & encoding ) {
					write( data, encoding );
					end( );
				}

				void HttpServerResponseImpl::close( ) {
					if( !send( ) ) {
						m_socket.close( );
					}
				}

				void HttpServerResponseImpl::reset( ) {
					m_status_sent = false;
					m_headers.headers.clear( );
					m_headers_sent = false;
					clear_body( );
					m_body_sent = false;
				}

				bool HttpServerResponseImpl::is_closed( ) const {
					return m_socket.is_closed( );
				}

				bool HttpServerResponseImpl::can_write( ) const {
					return m_socket.can_write( );
				}

				bool HttpServerResponseImpl::is_open( ) {
					return m_socket.is_open( );
				}

				HttpServerResponseImpl& HttpServerResponseImpl::add_header( std::string header_name, std::string header_value ) {
					m_headers.add( std::move( header_name ), std::move( header_value ) );
					return *this;
				}

				HttpServerResponseImpl& HttpServerResponseImpl::when_all_writes_complete( std::function<void( )> listener ) {
					add_listener( "finish", listener );
					return *this;
				}

				HttpServerResponseImpl& HttpServerResponseImpl::when_next_all_writes_complete( std::function<void( )> listener ) {
					add_listener( "finish", listener, true );
					return *this;
				}

				HttpServerResponseImpl& HttpServerResponseImpl::when_a_write_completes( std::function<void( )> listener ) {
					add_listener( "drain", listener );
					return *this;
				}

				HttpServerResponseImpl& HttpServerResponseImpl::when_next_write_completes( std::function<void( )> listener ) {
					add_listener( "drain", listener, true );
					return *this;
				}

				HttpServerResponse::~HttpServerResponse( ) { }

				HttpServerResponse::HttpServerResponse( lib::net::NetSocketStream socket ) : m_impl( std::make_shared<HttpServerResponseImpl>( std::move( socket ) ) ) { }

				HttpServerResponse::HttpServerResponse( HttpServerResponse && other ): m_impl( std::move( other.m_impl ) ) { }
				
				HttpServerResponse& HttpServerResponse::operator=(HttpServerResponse rhs) {
					m_impl = std::move( rhs.m_impl );
					return *this;
				}


				HttpServerResponse& HttpServerResponse::write( base::data_t const & data ) {
					m_impl->write( data );
					return *this;
				}

				HttpServerResponse& HttpServerResponse::write( std::string const & data, base::Encoding const & encoding ) {
					m_impl->write( data, encoding );
					return *this;
				}

				void HttpServerResponse::end( ) {
					m_impl->end( );
				}

				void HttpServerResponse::end( base::data_t const & data ) {
					m_impl->end( data );
				}

				void HttpServerResponse::end( std::string const & data, base::Encoding const & encoding ) {
					m_impl->end( data, encoding );
				}

				void HttpServerResponse::close( ) {
					m_impl->close( );
				}

				HttpHeaders& HttpServerResponse::headers( ) {
					return m_impl->headers( );
				}

				HttpHeaders const & HttpServerResponse::headers( ) const {
					return m_impl->headers( );
				}

				void HttpServerResponse::send_status( uint16_t status_code ) {
					m_impl->send_status( status_code );
				}
				void HttpServerResponse::send_headers( ) {
					m_impl->send_headers( );
				}

				void HttpServerResponse::send_body( ) {
					m_impl->send_body( );
				}

				void HttpServerResponse::clear_body( ) {
					m_impl->clear_body( );
				}

				bool HttpServerResponse::send( ) {
					return m_impl->send( );
				}

				void HttpServerResponse::reset( ) {
					m_impl->reset( );
				}

				bool HttpServerResponse::is_open( ) {
					return m_impl->is_open( );
				}

				bool HttpServerResponse::is_closed( ) const {
					return m_impl->is_closed( );
				}

				bool HttpServerResponse::can_write( ) const {
					return m_impl->can_write( );
				}

				HttpServerResponse& HttpServerResponse::add_header( std::string header_name, std::string header_value ) {
					m_impl->add_header( std::move( header_name ), std::move( header_value ) );
					return *this;
				}


				//////////////////////////////////////////////////////////////////////////
				/// Summary: Event emitted when a write is completed
				/// Inherited from StreamWritable
				HttpServerResponse& HttpServerResponse::when_a_write_completes( std::function<void( )> listener ) {
					m_impl->when_a_write_completes( listener );
					return *this;
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Event emitted when end( ... ) has been called and all data
				/// has been flushed
				/// Inherited from StreamWritable
				HttpServerResponse& HttpServerResponse::when_all_writes_complete( std::function<void( )> listener ) {
					m_impl->when_all_writes_complete( listener );
					return *this;
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Event emitted when end( ... ) has been called and all data
				/// has been flushed
				/// Inherited from StreamWritable
				HttpServerResponse& HttpServerResponse::when_next_all_writes_complete( std::function<void( )> listener ) {
					m_impl->when_next_all_writes_complete( listener );
					return *this;
				}

				//////////////////////////////////////////////////////////////////////////
				/// Summary: Event emitted when the next write is completed
				/// Inherited from StreamWritable
				HttpServerResponse& HttpServerResponse::when_next_write_completes( std::function<void( )> listener ) {
					m_impl->when_next_write_completes( listener );
					return *this;
				}


			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
