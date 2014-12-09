#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdint>
#include <mutex>
#include <string>

#include "base_enoding.h"
#include "base_error.h"
#include "base_event_emitter.h"
#include "base_service_handle.h"
#include "base_stream.h"
#include "base_types.h"
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
						auto local = std::vector < std::string > { "connect", "timeout" };
						return base::impl::append_vector( local, base::stream::Stream::valid_events( ) );
					}();
					return result;
				}

				NetSocket::NetSocket( boost::asio::io_service& io_service ) : base::stream::Stream( ),
																m_socket( io_service ),
																m_response_buffer( std::make_shared<boost::asio::streambuf>( ) ),
																m_response_buffers( std::make_shared<base::data_t>( ) ),
																m_bytes_read( 0 ),
																m_bytes_written( 0 ),
																m_response_buffers_mutex( ),
																m_read_mode( ReadUntil::newline ),
																m_read_predicate( ),
																m_outstanding_writes( 0 ),
																m_end( false ) { }
				
				NetSocket::NetSocket( SocketHandle handle ) : base::stream::Stream( ),
																m_socket( handle ),
																m_response_buffer( std::make_shared<boost::asio::streambuf>( ) ),
																m_response_buffers( std::make_shared<base::data_t>( ) ),
																m_bytes_read( 0 ),
																m_bytes_written( 0 ),
																m_response_buffers_mutex( ),
																m_read_mode( ReadUntil::newline ),
																m_read_predicate( ),
																m_outstanding_writes( 0 ),
																m_end( false ) { }

				NetSocket::NetSocket( NetSocket&& other ):	base::stream::Stream( std::move( other ) ),
															m_socket( std::move( other.m_socket ) ), 
															m_response_buffer( std::move( other.m_response_buffer ) ), 
															m_response_buffers( std::move( other.m_response_buffers ) ),
															m_bytes_read( std::move( other.m_bytes_read ) ),
															m_bytes_written( std::move( other.m_bytes_written ) ),
															m_response_buffers_mutex( ),
															m_read_mode( std::move( other.m_read_mode ) ),
															m_read_predicate( std::move( other.m_read_predicate ) ),
															m_outstanding_writes( std::move( other.m_outstanding_writes ) ),
															m_end( std::move( other.m_end ) ) { }

				NetSocket& NetSocket::operator=(NetSocket&& rhs) {
					if( this != &rhs ) {
						m_socket = std::move( rhs.m_socket );						
						m_response_buffer = std::move( rhs.m_response_buffer );
						m_response_buffers = std::move( rhs.m_response_buffers );
						m_bytes_read = std::move( rhs.m_bytes_read );
						m_bytes_written = std::move( rhs.m_bytes_written );
						m_read_mode = std::move( rhs.m_read_mode );
						m_read_predicate = std::move( rhs.m_read_predicate );
						m_outstanding_writes = std::move( rhs.m_outstanding_writes );
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
					
					void emit_data( NetSocket* const net_socket, std::shared_ptr<base::data_t> buffer, bool end_of_file ) {
						base::ServiceHandle::get( ).post( [net_socket, buffer, end_of_file]( ) mutable {
							net_socket->emit( "data", buffer, end_of_file );
							if( end_of_file ) {
								net_socket->emit( "end" );
							}
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
					remove_all_listeners( );
				}

				void NetSocket::inc_outstanding_writes( ) {
					m_outstanding_writes++;
				}
				bool NetSocket::dec_outstanding_writes( ) {
					return 0 == --m_outstanding_writes;
				}

				void NetSocket::do_async_read( ) {
					auto handler = boost::bind( &NetSocket::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred );
					
					switch( m_read_mode ) {
					case ReadUntil::next_byte:
						throw std::runtime_error( "Read Until mode not implemented" );
						//boost::asio::async_read( *m_socket, *m_response_buffer, boost::asio::transfer_exactly( 1 ), handler );
					case ReadUntil::buffer_full:
						boost::asio::async_read( *m_socket, *m_response_buffer, handler );
						break;
					case ReadUntil::newline:
						boost::asio::async_read_until( *m_socket, *m_response_buffer, "\n", handler );
						break;
					case ReadUntil::predicate:
						boost::asio::async_read_until( *m_socket, *m_response_buffer, *m_read_predicate, handler );
						break;
					default:
						throw std::runtime_error( "read until method not implemented" );
					}
					
				}

				NetSocket& NetSocket::set_read_mode( ReadUntil mode ) {
					m_read_mode = mode;
					return *this;
				}
				
				NetSocket::ReadUntil const& NetSocket::current_read_mode( ) const { 
					return m_read_mode;
				}

				NetSocket& NetSocket::set_read_predicate( match_function_t read_predicate ) { 
					m_read_predicate = std::make_shared<match_function_t>( read_predicate );
					m_read_mode = ReadUntil::predicate;
					return *this;
				}
				
				NetSocket& NetSocket::clear_read_predicate( ) { 
					if( ReadUntil::predicate == m_read_mode ) {
						m_read_mode = ReadUntil::newline;
					}
					m_read_predicate.reset( );
					return *this;
				}

				std::shared_ptr<base::data_t> get_clear_buffer( std::shared_ptr<base::data_t>& buffer, size_t num_items, size_t new_size = 1024 ) {
					std::shared_ptr<base::data_t> result( std::make_shared<base::data_t>( new_size, 0 ) );
					using std::swap;
					swap( result, buffer );
					result->resize( num_items );
					return result;
				}

				

				NetSocket& NetSocket::on_connect( std::function<void( )> listener ) {
					add_listener( "connect", listener );
					return *this;
				}

				NetSocket& NetSocket::on_error( std::function<void( base::Error )> listener ) {
					add_listener( "error", listener );
					return *this;
				}

				NetSocket& NetSocket::on_data( std::function<void( std::shared_ptr<base::data_t>, bool )> listener ) {
					add_listener( "data", listener );
					return *this;
				}

				NetSocket& NetSocket::on_end( std::function<void( )> listener ) {
					add_listener( "end", listener );
					return *this;
				}

				NetSocket& NetSocket::on_close( std::function<void( )> listener ) { 
					add_listener( "close", listener );
					return *this;
				}

				NetSocket& NetSocket::on_finish( std::function<void( )> listener ) { 
					add_listener( "finish", listener );
				}

				NetSocket& NetSocket::on_pipe( std::function<void( StreamReadable& )> listener ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocket& NetSocket::on_unpipe( std::function<void( StreamReadable& )> listener ) { throw std::runtime_error( "Method not implemented" ); }


				NetSocket& NetSocket::once_connect( std::function<void( )> listener ) {
					add_listener( "connect", listener, true );
					return *this;
				}

				NetSocket& NetSocket::once_error( std::function<void( base::Error )> listener ) {
					add_listener( "error", listener, true );
					return *this;
				}

				NetSocket& NetSocket::once_data( std::function<void( std::shared_ptr<base::data_t>, bool )> listener ) {
					add_listener( "data", listener, true );
					return *this;
				}

				NetSocket& NetSocket::once_end( std::function<void( )> listener ) {
					add_listener( "end", listener, true );
					return *this;
				}

				NetSocket& NetSocket::once_close( std::function<void( )> listener ) {
					add_listener( "close", listener, true );
					return *this;
				}

				NetSocket& NetSocket::once_finish( std::function<void( )> listener ) {
					add_listener( "finish", listener, true );
				}

				NetSocket& NetSocket::once_pipe( std::function<void( StreamReadable& )> listener ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocket& NetSocket::once_unpipe( std::function<void( StreamReadable& )> listener ) { throw std::runtime_error( "Method not implemented" ); }

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
					
					if( 0 < bytes_transfered ) {
						std::istream resp( m_response_buffer.get( ) );
						auto new_data = std::make_shared<base::data_t>( bytes_transfered );
						resp.read( reinterpret_cast<char*>( new_data->data( ) ), static_cast<std::streamsize>( bytes_transfered ) );
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

							emit_data( this, new_data, end_of_file );
						} else {	// Queue up for a			
							daw::copy_vect_and_set( new_data, m_response_buffers, bytes_transfered, static_cast<base::data_t::value_type>(0) );
						}
						m_bytes_read += bytes_transfered;
					}

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
					if( dec_outstanding_writes( ) && m_end ) {
						emit( "finish" );
					}
				} 

				boost::asio::ip::tcp::socket & NetSocket::socket( ) {
					return *m_socket;
				}

				boost::asio::ip::tcp::socket const & NetSocket::socket( ) const {
					return *m_socket;
				}

				NetSocket& NetSocket::write( impl::write_buffer buff ) {
					m_bytes_written += buff.size( );
					auto handler = boost::bind( &NetSocket::handle_write, this, buff, boost::asio::placeholders::error );
					inc_outstanding_writes( );
					boost::asio::async_write( *m_socket, buff.asio_buff( ), handler );
					return *this;
				}

				NetSocket& NetSocket::write( base::data_t const & chunk ) { 
					write( impl::write_buffer( chunk ) );
				}
				
				NetSocket& NetSocket::write( std::string const & chunk, base::Encoding const & encoding ) { 
					write( impl::write_buffer( chunk ) );
				}

				void NetSocket::end( ) { 
					//m_socket = std::make_shared<boost::asio::ip::tcp::socket>( base::ServiceHandle::get( ) );
					m_end = true;
					m_socket->shutdown( boost::asio::ip::tcp::socket::shutdown_send );					
				}

				void NetSocket::end( base::data_t const & chunk ) { 
					write( chunk );
					end( );
				}

				void NetSocket::end( std::string const & chunk, base::Encoding const & encoding ) {
					write( chunk, encoding );
					end( );
				}

				void NetSocket::close( ) {
					m_socket->close( );
					emit( "close" );
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
			

				NetSocket& operator<<(NetSocket& net_socket, std::string const & value ) {
					return net_socket.write( value );
				}

				NetSocket& operator<<(NetSocket& net_socket, base::data_t const & value) {
					return net_socket.write( value );
				}

			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
