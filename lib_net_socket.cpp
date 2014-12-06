#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdint>
#include <mutex>
#include <string>

#include "base_enoding.h"
#include "base_event_emitter.h"
#include "base_error.h"
#include "base_stream.h"
#include "base_types.h"
#include "base_service_handle.h"
#include "lib_net_dns.h"
#include "lib_net_socket.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				using namespace daw::nodepp;
				using namespace boost::asio::ip;

				namespace {
					template<typename Container>
					auto to_bbuffer( std::shared_ptr<Container>& container ) -> decltype(boost::asio::buffer( container->data( ), container->size( ) )) {
						if( container && !container->empty( ) ) {
							return boost::asio::buffer( container->data( ), container->size( ) );
						} else {

						}
					}
				}

				std::vector<std::string> const & NetSocket::valid_events( ) const {
					static auto const result = [&]( ) {
						auto local = std::vector < std::string > { "connect", "data", "end", "timeout", "drain", "error", "close", "endOfFile" };
						return base::impl::append_vector( local, base::stream::Stream::valid_events( ) );
					}();
					return result;
				}

				NetSocket::NetSocket( boost::asio::io_service& io_service ) : base::stream::Stream( ),
																m_socket( io_service ),
																m_response_buffer( std::make_shared<base::data_t>( 1024, 0 ) ),
																m_response_buffers( std::make_shared<base::data_t>( ) ),
																m_bytes_read( 0 ),
																m_bytes_written( 0 ),
																m_response_buffers_mutex( ) {
					std::cout << "Constructing default NetSocket" << std::endl;
				}
				
				NetSocket::NetSocket( SocketHandle handle ) : base::stream::Stream( ),
																m_socket( handle ),
																m_response_buffer( std::make_shared<base::data_t>( 1024, 0 ) ),
																m_response_buffers( std::make_shared<base::data_t>( ) ),
																m_bytes_read( 0 ),
																m_bytes_written( 0 ),
																m_response_buffers_mutex( ) {
					std::cout << "Constructing NetSocket from SocketHandle" << std::endl;
				}

				NetSocket::NetSocket( NetSocket&& other ):	base::stream::Stream( std::move( other ) ),
															m_socket( std::move( other.m_socket ) ), 
															m_response_buffer( std::move( other.m_response_buffer ) ), 
															m_response_buffers( std::move( other.m_response_buffers ) ),
															m_bytes_read( std::move( other.m_bytes_read ) ),
															m_bytes_written( std::move( other.m_bytes_written ) ),
															m_response_buffers_mutex( ) { }

				NetSocket& NetSocket::operator=(NetSocket&& rhs) {
					if( this != &rhs ) {
						m_socket = std::move( rhs.m_socket );						
						m_response_buffer = std::move( rhs.m_response_buffer );
						m_response_buffers = std::move( rhs.m_response_buffers );
						m_bytes_read = std::move( rhs.m_bytes_read );
						m_bytes_written = std::move( rhs.m_bytes_written );
					}
					return *this;
				}

				namespace {

					void emit_error( NetSocket* const net_socket, boost::system::error_code const & err, std::string where ) {
						auto error = base::Error( err );
						error.add( "where", where );
						base::ServiceHandle::get( ).post( [net_socket, error]( ) {
							net_socket->emit( "error", error );
						} );
					}

					void emit_end( NetSocket* const net_socket ) {
						base::ServiceHandle::get( ).post( [net_socket]( ) {
							net_socket->emit( "error" );
						} );
					}

					void emit_data( NetSocket* const net_socket, std::shared_ptr<base::data_t> buffer, bool end_of_file ) {
						base::ServiceHandle::get( ).post( [net_socket, buffer, end_of_file]( ) mutable {
							net_socket->emit( "data", buffer, end_of_file );
						} );
					}

					void connect_handler( NetSocket* const net_socket, boost::system::error_code const & err, tcp::resolver::iterator it ) {
						if( !err ) {
							base::ServiceHandle::get( ).post( [net_socket]( ) {
								net_socket->emit( "connect" );
							} );
						} else {
							emit_error( net_socket, err, "NetSocket::connect" );
						}
					}
				}	// namespace end

				NetSocket::~NetSocket( ) {
					m_socket->close( );
					emit_end( this );					
					remove_all_listeners( );
					std::cout << "Destructing socket @ " << std::hex << reinterpret_cast<uintptr_t>(this) << std::endl;
				}

				void NetSocket::do_async_read( ) {
					auto handler = boost::bind( &NetSocket::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred );
					boost::asio::async_read( *m_socket, to_bbuffer( m_response_buffer ), handler );
				}

				std::shared_ptr<base::data_t> get_clear_buffer( std::shared_ptr<base::data_t>& buffer, size_t num_items, size_t new_size = 1024 ) {
					std::shared_ptr<base::data_t> result( std::make_shared<base::data_t>( new_size, 0 ) );
					using std::swap;
					swap( result, buffer );
					result->resize( num_items );
					return result;
				}

				NetSocket& NetSocket::on_data( std::function<void( std::shared_ptr<base::data_t>, bool )> listener ) {
					add_listener( "data", listener );
					return *this;
				}

				NetSocket& NetSocket::on_connect( std::function<void( )> listener ) {
					add_listener( "connect", listener );
					return *this;
				}

				NetSocket& NetSocket::on_error( std::function<void(base::Error )> listener ) {
					add_listener( "error", listener );
					return *this;
				}

				NetSocket& NetSocket::on_end( std::function<void( )> listener ) {
					add_listener( "end", listener );
					return *this;
				}

				NetSocket& NetSocket::connect( std::string host, uint16_t port ) {
					tcp::resolver resolver( base::ServiceHandle::get( ) );
					auto handler = boost::bind( connect_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator );
					boost::asio::async_connect( m_socket.get( ), resolver.resolve( { host, boost::lexical_cast<std::string>(port) } ), handler );
					return *this;
				}

				NetSocket& NetSocket::connect( std::string path ) { throw std::runtime_error( "Method not implemented" ); }

				size_t& NetSocket::buffer_size( ) { throw std::runtime_error( "Method not implemented" ); }

				size_t const & NetSocket::buffer_size( ) const { throw std::runtime_error( "Method not implemented" ); }
				
				void NetSocket::handle_read( boost::system::error_code const & err, size_t bytes_transfered ) {
					auto net_socket = this;					
					if( 0 < listener_count( "data" ) ) {
						{
							// Handle when the emitter comes after the data starts pouring in.  This might be best placed in newEvent
							// have not decided
							std::lock_guard<std::mutex> scoped_lock( m_response_buffers_mutex );
							if( !m_response_buffers->empty( ) ) {
								emit_data( this, get_clear_buffer( m_response_buffers, m_response_buffers->size( ), 0 ), false );
							}
						}
						bool end_of_file = err && 2 == err.value( );
						emit_data( this, get_clear_buffer( m_response_buffer, bytes_transfered ), end_of_file );
					} else {	// Queue up for a 						
						daw::copy_vect_and_set( m_response_buffer, m_response_buffers, bytes_transfered, static_cast<base::data_t::value_type>(0) );
					}
					m_bytes_read += bytes_transfered;

					if( !err ) {
						do_async_read( );
					} else if( 2 != err.value( ) ) {
						emit_error( this, err, "NetSocket::read" );
					}
				}

				bool NetSocket::is_open( ) const {
					return m_socket && m_socket->is_open( );
				}

				void NetSocket::handle_write( impl::write_buffer buff, boost::system::error_code const & err ) {
					if( !err ) {
						if( is_open( ) ) {
							NetSocket::do_async_read( );
						}
					} else {
						emit_error( this, err, "NetSocket::write" );
					}
				} 

				boost::asio::ip::tcp::socket & NetSocket::socket( ) {
					return *m_socket;
				}

				boost::asio::ip::tcp::socket const & NetSocket::socket( ) const {
					return *m_socket;
				}

				NetSocket& NetSocket::write( base::data_t const & chunk ) { 
					auto buff = impl::write_buffer( chunk );
					m_bytes_written += buff.size( );
					auto handler = boost::bind( &NetSocket::handle_write, this, buff, boost::asio::placeholders::error );
					boost::asio::async_write( *m_socket, buff.asio_buff( ), handler );
					return *this;
				}
				
				NetSocket& NetSocket::write( std::string const & chunk, base::Encoding const & encoding ) { 
					auto buff = impl::write_buffer( chunk );
					m_bytes_written += buff.size( );
					auto handler = boost::bind( &NetSocket::handle_write, this, buff, boost::asio::placeholders::error );
					boost::asio::async_write( *m_socket, buff.asio_buff( ), handler );
					return *this;
				}

				NetSocket& NetSocket::end( ) { 
					//m_socket = std::make_shared<boost::asio::ip::tcp::socket>( base::ServiceHandle::get( ) );
					m_socket->shutdown( boost::asio::ip::tcp::socket::shutdown_send );
					return *this;
				}

				NetSocket& NetSocket::end( base::data_t const & chunk ) { 
					write( chunk );
					return end( );
				}

				NetSocket& NetSocket::end( std::string const & chunk, base::Encoding const & encoding ) {
					write( chunk, encoding );
					return end( );
				}

				NetSocket& NetSocket::destroy( ) {
					m_socket->close( );
					return *this;
				}
				
				NetSocket& NetSocket::set_timeout( int32_t value ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocket& NetSocket::set_no_delay( bool no_delay ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocket& NetSocket::set_keep_alive( bool keep_alive, int32_t initial_delay ) { throw std::runtime_error( "Method not implemented" ); }

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
				
				size_t NetSocket::bytes_read( ) const {
					return m_bytes_read;
				}

				size_t NetSocket::bytes_written( ) const {
					return m_bytes_written;
				}

				// StreamReadable Interface
				base::data_t NetSocket::read( ) { 
					std::shared_ptr<base::data_t> result;
					{
						std::lock_guard<std::mutex> scoped_lock( m_response_buffers_mutex );
						result = get_clear_buffer( m_response_buffers, m_response_buffers->size( ), 0 );
					}
					return *result;
				}

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
