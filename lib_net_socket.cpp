#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdint>
#include <string>

#include "base_enoding.h"
#include "base_event_emitter.h"
#include "base_error.h"
#include "base_stream.h"
#include "base_types.h"
#include "base_handle.h"
#include "lib_net_dns.h"
#include "lib_net_socket.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				using namespace daw::nodepp;
				using namespace boost::asio::ip;

				std::vector<std::string> const & NetSocket::valid_events( ) const {
					static auto const result = [&]( ) {
						auto local = std::vector < std::string > { "connect", "data", "end", "timeout", "drain", "error", "close" };
						return base::impl::append_vector( local, base::stream::Stream::valid_events( ) );
					}();
					return result;
				}

				boost::asio::streambuf & NetSocket::request_buffer( ) {
					return *m_request_buffer;
				}

				boost::asio::streambuf const & NetSocket::request_buffer( ) const {
					return *m_request_buffer;
				}

				boost::asio::streambuf & NetSocket::response_buffer( ) {
					return *m_response_buffer;
				}

				boost::asio::streambuf const & NetSocket::response_buffer( ) const {
					return *m_response_buffer	;
				}

				NetSocket::NetSocket( ) : base::stream::Stream( ), m_socket( std::make_shared<boost::asio::ip::tcp::socket>( base::Handle::get( ) ) ), m_request_buffer( std::make_shared<boost::asio::streambuf>( ) ), m_response_buffer( std::make_shared<boost::asio::streambuf>( ) ) { }
				
				NetSocket::NetSocket( NetSocket&& other ) : base::stream::Stream( std::move( other ) ), m_socket( std::move( other.m_socket ) ), m_request_buffer( std::move( other.m_request_buffer ) ), m_response_buffer( std::move( other.m_response_buffer ) ) { }

				NetSocket& NetSocket::operator=(NetSocket&& rhs) {
					if( this != &rhs ) {
						m_socket = std::move( rhs.m_socket );						
						m_request_buffer = std::move( rhs.m_request_buffer );
						m_response_buffer = std::move( rhs.m_response_buffer );
					}
					return *this;
				}

				NetSocket::~NetSocket( ) { }

				namespace {
					void connect_handler( NetSocket* const net_socket, boost::system::error_code const & err, tcp::resolver::iterator it ) {
						if( !err ) {
							base::Handle::get( ).post( [net_socket]( ) {
								net_socket->emit( "connect" );
							} );							
						} else {
							auto error = base::Error( err );
							error.add( "where", "NetSocket::connect" );
							base::Handle::get( ).post( [net_socket, error]( ) {																
								net_socket->emit( "error", error );
							} );
						}
					}
				}

				NetSocket& NetSocket::connect( std::string host, uint16_t port ) {
					auto resolver = tcp::resolver( base::Handle::get( ) );
					auto handler = boost::bind( connect_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator );
					boost::asio::async_connect( *m_socket, resolver.resolve( { host, boost::lexical_cast<std::string>(port) } ), handler );
					return *this;
				}

				NetSocket& NetSocket::connect( std::string path ) { throw std::runtime_error( "Method not implemented" ); }

				size_t& NetSocket::buffer_size( ) { throw std::runtime_error( "Method not implemented" ); }

				size_t const & NetSocket::buffer_size( ) const { throw std::runtime_error( "Method not implemented" ); }
				
				NetSocket& NetSocket::end( ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocket& NetSocket::end( base::data_t const & chunk ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocket& NetSocket::end( std::string chunk, base::Encoding const & encoding ) { throw std::runtime_error( "Method not implemented" ); }
				
				void NetSocket::handle_read( boost::system::error_code const & err ) {
					auto net_socket = this;
					if( !err ) {
						std::istream response_stream( m_response_buffer.get( ) );
						if( 0 < listener_count( "data" ) ) {
							
						} else {	// buffer until close

						}
					} else {
						auto error = base::Error( err );
						error.add( "where", "NetSocket::read" );
						base::Handle::get( ).post( [net_socket, error]( ) {
							net_socket->emit( "error", error );
						} );
					}
				}

				void NetSocket::handle_write( boost::system::error_code const & err ) {
					auto net_socket = this;
					if( !err ) {
						auto handler = boost::bind( &NetSocket::handle_read, this, boost::asio::placeholders::error );
						boost::asio::async_read( *m_socket, response_buffer( ), handler );
					} else {
						auto error = base::Error( err );
						error.add( "where", "NetSocket::write" );
						base::Handle::get( ).post( [net_socket, error]( ) {
							net_socket->emit( "error", error );
						} );
					}											
				}

				bool NetSocket::write( base::data_t const & chunk ) { throw std::runtime_error( "Method not implemented" ); }
				bool NetSocket::write( std::string data, base::Encoding const & encoding ) { throw std::runtime_error( "Method not implemented" ); }

				NetSocket& NetSocket::destroy( ) { throw std::runtime_error( "Method not implemented" ); }
				

				NetSocket& NetSocket::set_timeout( int32_t value ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocket& NetSocket::set_no_delay( bool no_delay ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocket& NetSocket::set_keep_alive( bool keep_alive, int32_t initial_delay ) { throw std::runtime_error( "Method not implemented" ); }

				lib::net::NetAddress const & NetSocket::address( ) const { throw std::runtime_error( "Method not implemented" ); }

				NetSocket& NetSocket::unref( ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocket& NetSocket::ref( ) { throw std::runtime_error( "Method not implemented" ); }

				std::string NetSocket::remote_address( ) const {
					return m_socket->remote_endpoint( ).address( ).to_string( );
				}

				std::string NetSocket::local_address( ) const { 
					return m_socket->local_endpoint( ).address( ).to_string( );
				}
				
				uint16_t NetSocket::remote_port( ) const { 
					return m_socket->remote_endpoint( ).port( );
				}

				uint16_t NetSocket::local_port( ) const { 
					return m_socket->local_endpoint( ).port( );
				}
				
				size_t NetSocket::bytes_read( ) const { throw std::runtime_error( "Method not implemented" ); }

				size_t NetSocket::bytes_written( ) const { throw std::runtime_error( "Method not implemented" ); }

				// StreamReadable Interface
				base::data_t  NetSocket::read( ) { throw std::runtime_error( "Method not implemented" ); }
				base::data_t  NetSocket::read( size_t bytes ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocket& NetSocket::set_encoding( base::Encoding const & encoding ) { throw std::runtime_error( "Method not implemented" ); }
				
				
				NetSocket& NetSocket::resume( ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocket& NetSocket::pause( ) { throw std::runtime_error( "Method not implemented" ); }

				using base::stream::StreamWritable;

				StreamWritable& NetSocket::pipe( StreamWritable& destination ) { throw std::runtime_error( "Method not implemented" ); }
				StreamWritable& NetSocket::pipe( StreamWritable& destination, base::options_t options ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocket& NetSocket::unpipe( StreamWritable& destination ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocket& NetSocket::unshift( base::data_t const & chunk ) { throw std::runtime_error( "Method not implemented" ); }
				
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
