#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <cstdint>
#include <string>

#include "base_enoding.h"
#include "base_error.h"
#include "base_event_emitter.h"
#include "base_service_handle.h"
#include "base_stream.h"
#include "base_types.h"
#include "lib_net_dns.h"
#include "range_algorithm.h"
#include "make_unique.h"

#include "lib_net_socket_stream.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				using namespace daw::nodepp;
				using namespace boost::asio::ip;


				//////////////////////////////////////////////////////////////////////////
				/// Helpers
				/// 

				namespace {
					base::data_t get_clear_buffer( base::data_t& original_buffer, size_t num_items, size_t new_size = 1024 ) {
						base::data_t new_buffer( new_size, 0 );
						using std::swap;
						swap( new_buffer, original_buffer );
						new_buffer.resize( num_items );
						return new_buffer;
					}
				}	// namespace anonymous
				
				NetSocketStream::NetSocketStream( std::shared_ptr<base::EventEmitter> emitter ) :
					base::StandardEvents<NetSocketStream>( emitter ), 
					base::stream::StreamReadableEvents<NetSocketStream>( emitter ), 
					base::stream::StreamWritableEvents<NetSocketStream>( emitter ),
					m_socket( base::ServiceHandle::get( ) ),
					m_emitter( std::move( emitter ) ),
					m_state( ),
					m_read_options( ),
					m_outstanding_writes( std::make_shared<std::atomic_int_least32_t>( 0 ) ),
					m_response_buffers( ),
					m_bytes_read( 0 ),
					m_bytes_written( 0 ) { }

				NetSocketStream::NetSocketStream( boost::asio::io_service& io_service, std::size_t max_read_size, std::shared_ptr<base::EventEmitter> emitter ) :
					base::StandardEvents<NetSocketStream>( emitter ),
					base::stream::StreamReadableEvents<NetSocketStream>( emitter ),
					base::stream::StreamWritableEvents<NetSocketStream>( emitter ),
					m_socket( io_service ),
					m_emitter( std::move( emitter ) ),
					m_state( ),
					m_read_options( max_read_size ),
					m_outstanding_writes( std::make_shared<std::atomic_int_least32_t>( 0 ) ),
					m_response_buffers( ),
					m_bytes_read( 0 ),
					m_bytes_written( 0 ) { }

				NetSocketStream::NetSocketStream( NetSocketStream && other ):
					base::StandardEvents<NetSocketStream>( std::move( other ) ),
					base::stream::StreamReadableEvents<NetSocketStream>( std::move( other ) ),
					base::stream::StreamWritableEvents<NetSocketStream>( std::move( other ) ),
					m_socket( std::move( other.m_socket ) ),
					m_emitter( std::move( other.m_emitter) ),
					m_state( std::move( other.m_state ) ),
					m_read_options( std::move( other.m_read_options ) ),
					m_outstanding_writes( std::move( other.m_outstanding_writes ) ),
					m_response_buffers( std::move( other.m_response_buffers ) ),
					m_bytes_read( std::move( other.m_bytes_read ) ),
					m_bytes_written( std::move( other.m_bytes_written ) ) { }

				NetSocketStream& NetSocketStream::operator=(NetSocketStream && rhs) {
					if( this != &rhs ) {
						m_socket = std::move( rhs.m_socket );
						m_emitter = std::move( rhs.m_emitter );
						m_state = std::move( rhs.m_state );
						m_read_options = std::move( rhs.m_read_options );
						m_outstanding_writes = std::move( rhs.m_outstanding_writes );
						m_response_buffers = std::move( rhs.m_response_buffers );
						m_bytes_read = std::move( rhs.m_bytes_read );
						m_bytes_written = std::move( rhs.m_bytes_written );
					}
					return *this;
				}

				NetSocketStream::~NetSocketStream( ) {
					// TODO: determine if we wait on outstanding_writes
					try {
						if( m_socket.is_open( ) ) {
							m_socket.close( );
						}
					} catch( std::exception const & ) {
						// Do nothing, we don't usually care.  It's gone, move on
					}
					std::cerr << "NetSocketStreamImpl::~NetSocketStreamImpl( )\n";
				}

				std::shared_ptr<NetSocketStream> NetSocketStream::get_ptr( ) {
					return shared_from_this( );
				}




				void NetSocketStream::set_read_mode( NetSocketStream::ReadUntil mode ) {
					m_read_options.read_mode = std::move( mode );
				}

				NetSocketStream::ReadUntil const& NetSocketStream::current_read_mode( ) const {
					return m_read_options.read_mode;
				}

				void NetSocketStream::set_read_predicate( NetSocketStream::match_function_t read_predicate ) {
					m_read_options.read_predicate = daw::make_unique<NetSocketStream::match_function_t>( read_predicate );
					m_read_options.read_mode = NetSocketStream::ReadUntil::predicate;
				}

				void NetSocketStream::clear_read_predicate( ) {
					if( ReadUntil::predicate == m_read_options.read_mode ) {
						m_read_options.read_mode = ReadUntil::newline;
					}
					m_read_options.read_until_values.clear( );
					m_read_options.read_predicate.reset( );
				}

				void NetSocketStream::set_read_until_values( std::string values, bool is_regex ) {
					m_read_options.read_mode = is_regex ? NetSocketStream::ReadUntil::regex : NetSocketStream::ReadUntil::values;
					m_read_options.read_until_values = std::move( values );
					m_read_options.read_predicate.reset( );
				}


				//////////////////////////////////////////////////////////////////////////


				bool NetSocketStream::dec_outstanding_writes( ) {
					return 0 == --(*m_outstanding_writes);
				}

				void NetSocketStream::inc_outstanding_writes( ) {
					(*m_outstanding_writes)++;
				}
				
				void NetSocketStream::handle_connect( std::shared_ptr<NetSocketStream> self, boost::system::error_code const & err, tcp::resolver::iterator it ) {
					if( !err ) {
						try {
							self->emit_connect( );
						} catch( ... ) {
							self->emit_error( std::current_exception( ), "Running connect listeners", "NetSocketStream::connect_handler" );
						}
					} else {
						self->emit_error( err, "NetSocketStream::connect" );
					}
				}

				void NetSocketStream::handle_read( std::shared_ptr<NetSocketStream> self, std::shared_ptr<boost::asio::streambuf> read_buffer, boost::system::error_code const & err, std::size_t bytes_transfered ) {
					try {
						read_buffer->commit( bytes_transfered );
						if( 0 < bytes_transfered ) {
							std::istream resp( read_buffer.get( ) );
							auto new_data = std::make_shared<base::data_t>( bytes_transfered );
							resp.read( new_data->data( ), static_cast<std::streamsize>(bytes_transfered) );
							read_buffer->consume( bytes_transfered );
							if( 0 < self->m_emitter->listener_count( "data" ) ) {

								{
									// Handle when the emitter comes after the data starts pouring in.  This might be best placed in newEvent
									// have not decided
									if( !self->m_response_buffers.empty( ) ) {
										auto buff = std::make_shared<base::data_t>( get_clear_buffer( self->m_response_buffers, self->m_response_buffers.size( ), 0 ) );
										self->emit_data_recv( std::move( buff ), false );
									}
								}
								bool end_of_file = err && 2 == err.value( );

								self->emit_data_recv( std::move( new_data ), end_of_file );
							} else {	// Queue up for a			
								daw::copy_vect_and_set( *new_data, self->m_response_buffers, bytes_transfered, static_cast<base::data_t::value_type>(0) );
							}
							self->m_bytes_read += bytes_transfered;
						}

						if( !err ) {
							if( !self->m_state.closed ) {
								self->read_async( read_buffer );
							}
						} else if( 2 != err.value( ) ) {
							self->emit_error( err, "NetSocket::read" );
						}
					} catch( std::exception const & ex ) {
						std::cerr << "Exception: " << ex.what( ) << " in read handler\n";
					}
				}

				void NetSocketStream::handle_write( std::shared_ptr<NetSocketStream> self, write_buffer buff, boost::system::error_code const & err, size_t bytes_transfered ) { // TODO see if we need buff, maybe lifetime issue
					self->m_bytes_written += bytes_transfered;
					if( !err ) {
						self->emit_write_completion( );
					} else {
						self->emit_error( err, "NetSocket::write" );
					}
					if( self->dec_outstanding_writes( ) ) {
						self->emit_all_writes_completed( );
					}
				}


				void NetSocketStream::emit_connect( ) {
					m_emitter->emit( "connect" );
				}

				void NetSocketStream::emit_timeout( ) {
					m_emitter->emit( "timeout" );
				}

				void NetSocketStream::write_async( write_buffer buff ) {
					if( m_state.closed || m_state.end ) {
						throw std::runtime_error( "Attempt to use a closed NetSocketStreamImpl" );
					}
					m_bytes_written += buff.size( );

					auto self = get_ptr( );
					auto handler = [self, buff]( boost::system::error_code const & err, size_t bytes_transfered ) mutable {
						self->handle_write( self, buff, err, bytes_transfered );
					};

					inc_outstanding_writes( );
					boost::asio::async_write( m_socket, buff.asio_buff( ), handler );
				}

				

				void NetSocketStream::read_async( std::shared_ptr<boost::asio::streambuf> read_buffer ) {
					if( m_state.closed ) {
						return;
					}
					if( !read_buffer ) {
						read_buffer = std::make_shared<boost::asio::streambuf>( m_read_options.max_read_size );
					}

					auto self = get_ptr( );
					auto handler = [self, read_buffer]( boost::system::error_code const & err, std::size_t bytes_transfered ) mutable {
						self->handle_read( self, read_buffer, err, bytes_transfered );
					};

					switch( m_read_options.read_mode ) {
					case ReadUntil::next_byte:
						throw std::runtime_error( "Read Until mode not implemented" );
					case ReadUntil::buffer_full:
						boost::asio::async_read( m_socket, *read_buffer, handler );
						break;
					case ReadUntil::newline:
						boost::asio::async_read_until( m_socket, *read_buffer, "\n", handler );
						break;
					case ReadUntil::predicate:
						boost::asio::async_read_until( m_socket, *read_buffer, *m_read_options.read_predicate, handler );
						break;
					case ReadUntil::values:
						boost::asio::async_read_until( m_socket, *read_buffer, m_read_options.read_until_values, handler );
						break;
					case ReadUntil::regex:
						boost::asio::async_read_until( m_socket, *read_buffer, boost::regex( m_read_options.read_until_values ), handler );
						break;

					default:
						throw std::runtime_error( "read until method not implemented" );
					}

				}

				
				void NetSocketStream::on_connected( std::function<void( )> listener ) {
					m_emitter->add_listener( "connect", listener );
				}

				void NetSocketStream::on_next_connected( std::function<void( )> listener ) {
					m_emitter->add_listener( "connect", listener, true );
				}


				void NetSocketStream::connect( std::string host, uint16_t port ) {
					tcp::resolver resolver( base::ServiceHandle::get( ) );

					auto self = get_ptr( );
					auto handler = [self]( boost::system::error_code const & err, tcp::resolver::iterator it ) {
						self->handle_connect( self, err, it );
					};

					boost::asio::async_connect( m_socket, resolver.resolve( { host, boost::lexical_cast<std::string>(port) } ), handler );
				}

				void NetSocketStream::connect( std::string path ) { throw std::runtime_error( "Method not implemented" ); }

				std::size_t& NetSocketStream::buffer_size( ) { throw std::runtime_error( "Method not implemented" ); }

				bool NetSocketStream::is_open( ) const {
					return m_socket.is_open( );
				}

				boost::asio::ip::tcp::socket & NetSocketStream::socket( ) {
					return m_socket;
				}

				void NetSocketStream::write_async( base::data_t const & chunk ) {
					write_async( write_buffer( chunk ) );
				}

				void NetSocketStream::write_async( boost::string_ref chunk, base::Encoding const & ) {
					write_async( write_buffer( chunk ) );
				}

				void NetSocketStream::end( ) {
					m_state.end = true;
					try {
						m_socket.shutdown( boost::asio::ip::tcp::socket::shutdown_send );
					} catch( ... ) {
						emit_error( std::current_exception( ), "Error calling shutdown on socket", "NetSocketStreamImpl::end( )" );
					}
				}

				void NetSocketStream::end( base::data_t const & chunk ) {
					write_async( chunk );
					end( );
				}

				void NetSocketStream::end( boost::string_ref chunk, base::Encoding const & encoding ) {
					write_async( chunk, encoding );
					end( );
				}

				void NetSocketStream::close( bool emit_cb ) {
					m_state.closed = true;
					m_state.end = true;
					try {
						m_socket.shutdown( boost::asio::ip::tcp::socket::shutdown_both );
					} catch( ... ) {
						emit_error( std::current_exception( ), "Error calling shutdown on socket", "NetSocketStreamImpl::close( )" );
					}
					if( emit_cb ) {
						emit_closed( );
					}
				}

				void NetSocketStream::cancel( ) {
					m_socket.cancel( );
				}

				void NetSocketStream::set_timeout( int32_t ) { throw std::runtime_error( "Method not implemented" ); }

				void NetSocketStream::set_no_delay( bool ) { throw std::runtime_error( "Method not implemented" ); }

				void NetSocketStream::set_keep_alive( bool, int32_t ) { throw std::runtime_error( "Method not implemented" ); }

				std::string NetSocketStream::remote_address( ) const {
					return m_socket.remote_endpoint( ).address( ).to_string( );
				}

				std::string NetSocketStream::local_address( ) const {
					return m_socket.local_endpoint( ).address( ).to_string( );
				}

				uint16_t NetSocketStream::remote_port( ) const {
					return m_socket.remote_endpoint( ).port( );
				}

				uint16_t NetSocketStream::local_port( ) const {
					return m_socket.local_endpoint( ).port( );
				}

				std::size_t NetSocketStream::bytes_read( ) const {
					return m_bytes_read;
				}

				std::size_t NetSocketStream::bytes_written( ) const {
					return m_bytes_written;
				}

				// StreamReadable Interface
				base::data_t NetSocketStream::read( ) {
					return get_clear_buffer( m_response_buffers, m_response_buffers.size( ), 0 );
				}

				base::data_t  NetSocketStream::read( std::size_t ) { throw std::runtime_error( "Method not implemented" ); }


				bool NetSocketStream::is_closed( ) const {
					return m_state.closed;
				}

				bool NetSocketStream::can_write( ) const {
					return !m_state.end;
				}

				//////////////////////////////////////////////////////////////////////////
				/// Write  Buffer
				///

				write_buffer::write_buffer( base::data_t const & source ) : buff( std::make_shared<base::data_t>( source ) ) { }

				write_buffer::write_buffer( boost::string_ref source ) : buff( std::make_shared<base::data_t>( source.begin( ), source.end( ) ) ) { }

				std::size_t write_buffer::size( ) const {
					return buff->size( );
				}

				write_buffer::data_type write_buffer::data( ) const {
					return buff->data( );
				}

				boost::asio::mutable_buffers_1 write_buffer::asio_buff( ) const {
					return boost::asio::buffer( data( ), size( ) );
				}

			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
