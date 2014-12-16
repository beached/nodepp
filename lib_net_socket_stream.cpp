#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
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
#include "lib_net_socket_stream.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				using namespace daw::nodepp;
				using namespace boost::asio::ip;

				namespace impl {

					write_buffer::write_buffer( base::data_t const & source ) : buff( std::make_shared<base::data_t>( source ) ) { }

					write_buffer::write_buffer( std::string const & source ) : buff( std::make_shared<base::data_t>( source.begin( ), source.end( ) ) ) { }

					size_t write_buffer::size( ) const {
						return buff->size( );
					}

					write_buffer::data_type write_buffer::data( ) const {
						return buff->data( );
					}

					boost::asio::mutable_buffers_1 write_buffer::asio_buff( ) const {
						return boost::asio::buffer( data( ), size( ) );
					}
				}

				std::vector<std::string> const & NetSocketStream::valid_events( ) const {
					static auto const result = [&]( ) {
						auto local = std::vector < std::string > { "connect", "timeout" };
						return base::impl::append_vector( local, base::stream::Stream::valid_events( ) );
					}();
					return result;
				}

				NetSocketStream::NetSocketStream( boost::asio::io_service& io_service, size_t max_read_size ) : base::stream::Stream( ),
					m_socket( io_service ),
					m_response_buffer( std::make_shared<boost::asio::streambuf>( max_read_size ) ),
					m_response_buffers( std::make_shared<base::data_t>( ) ),
					m_bytes_read( 0 ),
					m_bytes_written( 0 ),
					m_response_buffers_mutex( ),
					m_read_mode( ReadUntil::newline ),
					m_read_predicate( ),
					m_outstanding_writes( std::make_shared<std::atomic_int_least32_t>( 0 ) ),
					m_closed( false ),
					m_end( false ),
					m_read_until_values( ) { }

				NetSocketStream::NetSocketStream( NetSocketStream&& other ) : base::stream::Stream( std::move( other ) ),
					m_socket( std::move( other.m_socket ) ),
					m_response_buffer( std::move( other.m_response_buffer ) ),
					m_response_buffers( std::move( other.m_response_buffers ) ),
					m_bytes_read( std::move( other.m_bytes_read ) ),
					m_bytes_written( std::move( other.m_bytes_written ) ),
					m_response_buffers_mutex( ),
					m_read_mode( std::move( other.m_read_mode ) ),
					m_read_predicate( std::move( other.m_read_predicate ) ),
					m_outstanding_writes( std::move( other.m_outstanding_writes ) ),
					m_closed( std::move( other.m_closed ) ),
					m_end( std::move( other.m_end ) ),					
					m_read_until_values( std::move( other.m_read_until_values ) ) { }

				NetSocketStream& NetSocketStream::operator=(NetSocketStream&& rhs) {
					if( this != &rhs ) {
						m_socket = std::move( rhs.m_socket );
						m_response_buffer = std::move( rhs.m_response_buffer );
						m_response_buffers = std::move( rhs.m_response_buffers );
						m_bytes_read = std::move( rhs.m_bytes_read );
						m_bytes_written = std::move( rhs.m_bytes_written );
						m_read_mode = std::move( rhs.m_read_mode );
						m_read_predicate = std::move( rhs.m_read_predicate );
						m_outstanding_writes = std::move( rhs.m_outstanding_writes );
						m_closed = std::move( rhs.m_closed );
						m_end = std::move( rhs.m_end );
						m_read_until_values = std::move( rhs.m_read_until_values );
					}
					return *this;
				}

				namespace {

					void emit_error( NetSocketStream * const net_socket, boost::system::error_code const & err, std::string where ) {
						auto error = base::Error( err );
						error.add( "where", where );
						net_socket->emit( "error", error );
					}

					void emit_error( NetSocketStream * const net_socket, std::exception_ptr ex, std::string description, std::string where ) {
						auto error = base::Error( description, ex );
						error.add( "where", where );
						net_socket->emit( "error", error );
					}


					void emit_data( NetSocketStream* const net_socket, std::shared_ptr<base::data_t> buffer, bool end_of_file ) {
						net_socket->emit( "data", std::move( buffer ), end_of_file );
						if( end_of_file ) {
							net_socket->emit( "end" );
						}
					}

					void connect_handler( NetSocketStream* const net_socket, boost::system::error_code const & err, tcp::resolver::iterator it ) {
						if( !err ) {
							net_socket->emit( "connect" );
						} else {
							emit_error( net_socket, err, "NetSocket::connect" );
						}
					}
				}	// namespace end

				void NetSocketStream::inc_outstanding_writes( ) {
					(*m_outstanding_writes)++;
				}

				bool NetSocketStream::dec_outstanding_writes( ) {
					return 0 == --(*m_outstanding_writes);
				}

				void NetSocketStream::read_async( ) {
					if( m_closed ) {
						return;
					}
					auto handler = boost::bind( &NetSocketStream::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred );

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
					case ReadUntil::values:
						boost::asio::async_read_until( *m_socket, *m_response_buffer, m_read_until_values, handler );
						break;
					case ReadUntil::regex:
						boost::asio::async_read_until( *m_socket, *m_response_buffer, boost::regex(m_read_until_values), handler );
						break;

					default:
						throw std::runtime_error( "read until method not implemented" );
					}

				}

				NetSocketStream& NetSocketStream::set_read_mode( ReadUntil mode ) {
					m_read_mode = mode;
					return *this;
				}

				NetSocketStream::ReadUntil const& NetSocketStream::current_read_mode( ) const {
					return m_read_mode;
				}

				NetSocketStream& NetSocketStream::set_read_predicate( match_function_t read_predicate ) {
					m_read_predicate = std::make_shared<match_function_t>( read_predicate );
					m_read_mode = ReadUntil::predicate;
					return *this;
				}

				NetSocketStream& NetSocketStream::clear_read_predicate( ) {
					if( ReadUntil::predicate == m_read_mode ) {
						m_read_mode = ReadUntil::newline;
					}
					m_read_until_values = "";
					m_read_predicate.reset( );
					return *this;
				}

				NetSocketStream& NetSocketStream::set_read_until_values( std::string const & values, bool is_regex ) {
					m_read_mode = is_regex ? ReadUntil::regex : ReadUntil::values;
					m_read_until_values = values;
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

				NetSocketStream& NetSocketStream::on_connect( std::function<void( )> listener ) {
					add_listener( "connect", listener );
					return *this;
				}

				NetSocketStream& NetSocketStream::on_error( std::function<void( base::Error )> listener ) {
					add_listener( "error", listener );
					return *this;
				}

				NetSocketStream& NetSocketStream::on_data( std::function<void( std::shared_ptr<base::data_t>, bool )> listener ) {
					add_listener( "data", listener );
					return *this;
				}

				NetSocketStream& NetSocketStream::on_end( std::function<void( )> listener ) {
					add_listener( "end", listener );
					return *this;
				}

				NetSocketStream& NetSocketStream::on_close( std::function<void( )> listener ) {
					add_listener( "close", listener );
					return *this;
				}

				NetSocketStream& NetSocketStream::on_drain( std::function<void( )> listener ) {
					add_listener( "drain", listener );
					return *this;
				}

				NetSocketStream& NetSocketStream::on_finish( std::function<void( )> listener ) {
					add_listener( "finish", listener );
					return *this;
				}

				NetSocketStream& NetSocketStream::once_connect( std::function<void( )> listener ) {
					add_listener( "connect", listener, true );
					return *this;
				}

				NetSocketStream& NetSocketStream::once_error( std::function<void( base::Error )> listener ) {
					add_listener( "error", listener, true );
					return *this;
				}

				NetSocketStream& NetSocketStream::once_data( std::function<void( std::shared_ptr<base::data_t>, bool )> listener ) {
					add_listener( "data", listener, true );
					return *this;
				}

				NetSocketStream& NetSocketStream::once_end( std::function<void( )> listener ) {
					add_listener( "end", listener, true );
					return *this;
				}

				NetSocketStream& NetSocketStream::once_close( std::function<void( )> listener ) {
					add_listener( "close", listener, true );
					return *this;
				}

				NetSocketStream& NetSocketStream::once_finish( std::function<void( )> listener ) {
					add_listener( "finish", listener, true );
					return *this;
				}

				NetSocketStream& NetSocketStream::connect( std::string host, uint16_t port ) {
					tcp::resolver resolver( base::ServiceHandle::get( ) );
					auto handler = boost::bind( connect_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator );
					boost::asio::async_connect( m_socket.get( ), resolver.resolve( { host, boost::lexical_cast<std::string>(port) } ), handler );
					return *this;
				}

				NetSocketStream& NetSocketStream::connect( std::string path ) { throw std::runtime_error( "Method not implemented" ); }

				size_t& NetSocketStream::buffer_size( ) { throw std::runtime_error( "Method not implemented" ); }

				size_t const & NetSocketStream::buffer_size( ) const { throw std::runtime_error( "Method not implemented" ); }

				void NetSocketStream::handle_read( boost::system::error_code const & err, size_t bytes_transfered ) {

					if( 0 < bytes_transfered ) {
						std::istream resp( m_response_buffer.get( ) );
						auto new_data = std::make_shared<base::data_t>( bytes_transfered );
						resp.read( reinterpret_cast<char*>(new_data->data( )), static_cast<std::streamsize>(bytes_transfered) );
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

							emit_data( this, std::move( new_data ), end_of_file );
						} else {	// Queue up for a			
							daw::copy_vect_and_set( new_data, m_response_buffers, bytes_transfered, static_cast<base::data_t::value_type>(0) );
						}
						m_bytes_read += bytes_transfered;
					}

					if( !err ) {
						if( !m_closed ) {
							read_async( );
						}
					} else if( 2 != err.value( ) ) {
						emit_error( this, err, "NetSocket::read" );
					}
				}

				bool NetSocketStream::is_open( ) const {
					return m_socket && m_socket->is_open( );
				}

				void NetSocketStream::handle_write( impl::write_buffer buff, boost::system::error_code const & err ) { // TODO see if we need buff, maybe lifetime issue
					if( !err ) {
						emit( "drain" );
					} else {
						emit_error( this, err, "NetSocket::write" );
					}
					if( dec_outstanding_writes( ) && m_end ) {
						emit( "finish" );
					}
				}

				boost::asio::ip::tcp::socket & NetSocketStream::socket( ) {
					return *m_socket;
				}

				boost::asio::ip::tcp::socket const & NetSocketStream::socket( ) const {
					return *m_socket;
				}

				NetSocketStream& NetSocketStream::write( impl::write_buffer buff ) {
					if( m_closed || m_end ) {
						throw std::runtime_error( "Attempt to use a closed NetSocketStream" );
					}
					m_bytes_written += buff.size( );
					auto handler = boost::bind( &NetSocketStream::handle_write, this, buff, boost::asio::placeholders::error );
					inc_outstanding_writes( );
					boost::asio::async_write( *m_socket, buff.asio_buff( ), handler );
					return *this;
				}

				NetSocketStream& NetSocketStream::write( base::data_t const & chunk ) {
					return write( impl::write_buffer( chunk ) );
				}

				NetSocketStream& NetSocketStream::write( std::string const & chunk, base::Encoding const & ) {
					return write( impl::write_buffer( chunk ) );
				}

				void NetSocketStream::end( ) {
					m_end = true;
					try {
						m_socket->shutdown( boost::asio::ip::tcp::socket::shutdown_send );
					} catch( ... ) {
						emit_error( this, std::current_exception( ), "Error calling shutdown on socket", "NetSocketStream::end( )" );
					}
				}

				void NetSocketStream::end( base::data_t const & chunk ) {
					write( chunk );
					end( );
				}

				void NetSocketStream::end( std::string const & chunk, base::Encoding const & encoding ) {
					write( chunk, encoding );
					end( );
				}

				void NetSocketStream::close( bool emit_cb ) {
					m_closed = true;
					m_end = true;
					try {
						m_socket->shutdown( boost::asio::ip::tcp::socket::shutdown_both );
					} catch( ... ) {
						emit_error( this, std::current_exception( ), "Error calling shutdown on socket", "NetSocketStream::close( )" );
					}
					if( emit_cb ) {
						emit( "close" );
					}
				}

				void NetSocketStream::cancel( ) {
					m_socket->cancel( );
				}

				NetSocketStream& NetSocketStream::set_timeout( int32_t ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocketStream& NetSocketStream::set_no_delay( bool ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocketStream& NetSocketStream::set_keep_alive( bool, int32_t ) { throw std::runtime_error( "Method not implemented" ); }

				NetSocketStream& NetSocketStream::unref( ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocketStream& NetSocketStream::ref( ) { throw std::runtime_error( "Method not implemented" ); }

				std::string NetSocketStream::remote_address( ) const {
					return m_socket->remote_endpoint( ).address( ).to_string( );
				}

				std::string NetSocketStream::local_address( ) const {
					return m_socket->local_endpoint( ).address( ).to_string( );
				}

				uint16_t NetSocketStream::remote_port( ) const {
					return m_socket->remote_endpoint( ).port( );
				}

				uint16_t NetSocketStream::local_port( ) const {
					return m_socket->local_endpoint( ).port( );
				}

				size_t NetSocketStream::bytes_read( ) const {
					return m_bytes_read;
				}

				size_t NetSocketStream::bytes_written( ) const {
					return m_bytes_written;
				}

				// StreamReadable Interface
				base::data_t NetSocketStream::read( ) {
					std::shared_ptr<base::data_t> result;
					{
						std::lock_guard<std::mutex> scoped_lock( m_response_buffers_mutex );
						result = get_clear_buffer( m_response_buffers, m_response_buffers->size( ), 0 );
					}
					return *result;
				}

				base::data_t  NetSocketStream::read( size_t ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocketStream& NetSocketStream::set_encoding( base::Encoding const & ) { throw std::runtime_error( "Method not implemented" ); }


				NetSocketStream& NetSocketStream::resume( ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocketStream& NetSocketStream::pause( ) { throw std::runtime_error( "Method not implemented" ); }

				using base::stream::StreamWritable;

				StreamWritable& NetSocketStream::pipe( StreamWritable& ) { throw std::runtime_error( "Method not implemented" ); }
				StreamWritable& NetSocketStream::pipe( StreamWritable&, base::options_t ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocketStream& NetSocketStream::unpipe( StreamWritable& ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocketStream& NetSocketStream::unshift( base::data_t const & ) { throw std::runtime_error( "Method not implemented" ); }


				NetSocketStream& operator<<(NetSocketStream& net_socket, std::string const & value) {
					return net_socket.write( value );
				}

				NetSocketStream& operator<<(NetSocketStream& net_socket, base::data_t const & value) {
					return net_socket.write( value );
				}

				bool NetSocketStream::is_closed( ) const {
					return m_closed;
				}

				bool NetSocketStream::can_write( ) const {
					return !m_end;
				}

			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
