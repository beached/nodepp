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

				NetSocket::NetSocket( ) : base::stream::Stream( ), m_socket( std::make_shared<boost::asio::ip::tcp::socket>( base::Handle::get( ) ) ), m_endpoint( ) { }
				
				NetSocket::NetSocket( NetSocket&& other ) : base::stream::Stream( std::move( other ) ), m_socket( std::move( other.m_socket ) ), m_endpoint( std::move( other.m_endpoint ) ) { }

				NetSocket& NetSocket::operator=(NetSocket&& rhs) {
					if( this != &rhs ) {
						m_socket = std::move( rhs.m_socket );
						m_endpoint = std::move( rhs.m_endpoint );
					}
					return *this;
				}

				NetSocket::~NetSocket( ) { }


				namespace {
					void connect_handler( NetSocket* const net_socket, boost::system::error_code const & err, tcp::resolver::iterator ) {
						boost::asio::ip::tcp::resolver::iterator end;
						if( !err ) {
							net_socket->emit( "connect" );
						} else 							
							base::Error error( err );
							error.add( "where", "NetSocket::connect" );							
							net_socket->emit( "error", error );
						}
					}
				}


				NetSocket& NetSocket::connect( uint16_t port, std::string host ) { 
					auto dns = NetDns( );
					dns.on( "error", [&]( base::Error const & dns_error ) {
						auto error = base::Error( "see child" );
						error.set_child( dns_error );
						error.add( "where", "NetSocket::connect" );
						emit( "error", error )
					} ).once( "resolved", [&]( tcp::resolver::iterator it ) {
						auto handler = boost::bind( connect_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator );
						boost::asio::async_connect( *m_socket, it, handler );
					} );
					return *this;
				}

				NetSocket& NetSocket::connect( std::string path ) { throw std::runtime_error( "Method not implemented" ); }

				size_t& NetSocket::buffer_size( ) { throw std::runtime_error( "Method not implemented" ); }

				size_t const & NetSocket::buffer_size( ) const { throw std::runtime_error( "Method not implemented" ); }
				
				NetSocket& NetSocket::end( ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocket& NetSocket::end( base::data_t const & chunk ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocket& NetSocket::end( std::string chunk, base::Encoding const & encoding ) { throw std::runtime_error( "Method not implemented" ); }

				bool NetSocket::write( base::data_t const & chunk ) { throw std::runtime_error( "Method not implemented" ); }
				bool NetSocket::write( std::string data, base::Encoding const & encoding ) { throw std::runtime_error( "Method not implemented" ); }

				NetSocket& NetSocket::destroy( ) { throw std::runtime_error( "Method not implemented" ); }
				

				NetSocket& NetSocket::set_timeout( int32_t value ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocket& NetSocket::set_no_delay( bool no_delay ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocket& NetSocket::set_keep_alive( bool keep_alive, int32_t initial_delay ) { throw std::runtime_error( "Method not implemented" ); }

				lib::net::NetAddress const & NetSocket::address( ) const { throw std::runtime_error( "Method not implemented" ); }

				NetSocket& NetSocket::unref( ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocket& NetSocket::ref( ) { throw std::runtime_error( "Method not implemented" ); }

				std::string const & NetSocket::remote_address( ) const {
					return m_socket->remote_endpoint( ).address( ).to_string( );
				}

				std::string const & NetSocket::local_address( ) const { 
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
