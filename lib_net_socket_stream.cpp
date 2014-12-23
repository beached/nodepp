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
				namespace impl {

					namespace {
						base::data_t get_clear_buffer( base::data_t& original_buffer, size_t num_items, size_t new_size = 1024 ) {
							base::data_t new_buffer( new_size, 0 );
							using std::swap;
							swap( new_buffer, original_buffer );
							new_buffer.resize( num_items );
							return new_buffer;
						}
					}	// namespace anonymous

					NetSocketStreamImpl::NetSocketStreamImpl( base::EventEmitter emitter ) :
						m_socket( base::ServiceHandle::get( ) ),
						m_emitter( std::move( emitter ) ),
						m_state( ),
						m_read_options( ),
						m_outstanding_writes( std::make_shared<std::atomic_int_least32_t>( 0 ) ),
						m_response_buffers( ),
						m_bytes_read( 0 ),
						m_bytes_written( 0 ) { }

					NetSocketStreamImpl::NetSocketStreamImpl( boost::asio::io_service& io_service, std::size_t max_read_size, base::EventEmitter emitter ) :
						m_socket( io_service ),
						m_emitter( std::move( emitter ) ),
						m_state( ),
						m_read_options( max_read_size ),
						m_outstanding_writes( std::make_shared<std::atomic_int_least32_t>( 0 ) ),
						m_response_buffers( ),
						m_bytes_read( 0 ),
						m_bytes_written( 0 ) { }

					NetSocketStreamImpl::NetSocketStreamImpl( NetSocketStreamImpl && other ) :
						m_socket( std::move( other.m_socket ) ),
						m_emitter( std::move( other.m_emitter ) ),
						m_state( std::move( other.m_state ) ),
						m_read_options( std::move( other.m_read_options ) ),
						m_outstanding_writes( std::move( other.m_outstanding_writes ) ),
						m_response_buffers( std::move( other.m_response_buffers ) ),
						m_bytes_read( std::move( other.m_bytes_read ) ),
						m_bytes_written( std::move( other.m_bytes_written ) ) { }

					NetSocketStreamImpl& NetSocketStreamImpl::operator=(NetSocketStreamImpl && rhs) {
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

					NetSocketStreamImpl::~NetSocketStreamImpl( ) {
						// TODO: determine if we wait on outstanding_writes
						try {
							if( m_socket.is_open( ) ) {
								m_socket.close( );
							}
						} catch( ... ) {
							// Do nothing, we don't usually care.  It's gone, move on
						}
					}

// 					std::shared_ptr<NetSocketStreamImpl> NetSocketStreamImpl::get_ptr( ) {
// 						return shared_from_this( );
// 					}

					base::EventEmitter& NetSocketStreamImpl::emitter( ) {
						return m_emitter;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::set_read_mode( NetSocketStreamImpl::ReadUntil mode ) {
						m_read_options.read_mode =  mode;
						return *this;
					}

					NetSocketStreamImpl::ReadUntil const& NetSocketStreamImpl::current_read_mode( ) const {
						return m_read_options.read_mode;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::set_read_predicate( NetSocketStreamImpl::match_function_t read_predicate ) {
						m_read_options.read_predicate = daw::make_unique<NetSocketStreamImpl::match_function_t>( read_predicate );
						m_read_options.read_mode = NetSocketStreamImpl::ReadUntil::predicate;
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::clear_read_predicate( ) {
						if( ReadUntil::predicate == m_read_options.read_mode ) {
							m_read_options.read_mode = ReadUntil::newline;
						}
						m_read_options.read_until_values.clear( );
						m_read_options.read_predicate.reset( );
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::set_read_until_values( std::string values, bool is_regex ) {
						m_read_options.read_mode = is_regex ? NetSocketStreamImpl::ReadUntil::regex : NetSocketStreamImpl::ReadUntil::values;
						m_read_options.read_until_values = std::move( values );
						m_read_options.read_predicate.reset( );
						return *this;
					}

					bool NetSocketStreamImpl::dec_outstanding_writes( ) {
						return 0 == --(*m_outstanding_writes);
					}

					void NetSocketStreamImpl::inc_outstanding_writes( ) {
						(*m_outstanding_writes)++;
					}

					void NetSocketStreamImpl::handle_connect( std::weak_ptr<NetSocketStreamImpl> obj, boost::system::error_code const & err, tcp::resolver::iterator it ) {
						run_if_valid( obj, "Exception while connecting", "NetSocketStreamImpl::handle_connect", [&]( std::shared_ptr<NetSocketStreamImpl> & self ) {
							if( !err ) {
								try {
									self->emit_connect( );
								} catch( ... ) {
									self->emit_error( std::current_exception( ), "Running connect listeners", "NetSocketStreamImpl::connect_handler" );
								}
							} else {
								self->emit_error( err, "NetSocketStreamImpl::connect" );
							}
						} );
					}

					void NetSocketStreamImpl::handle_read( std::weak_ptr<NetSocketStreamImpl> obj, std::shared_ptr<boost::asio::streambuf> read_buffer, boost::system::error_code const & err, std::size_t bytes_transfered ) {
						run_if_valid( obj, "Exception while handling read", "NetSocketStreamImpl::handle_read", [&]( std::shared_ptr<NetSocketStreamImpl>& self ) {
							auto& response_buffers = self->m_response_buffers;

							read_buffer->commit( bytes_transfered );
							if( 0 < bytes_transfered ) {
								std::istream resp( read_buffer.get( ) );
								auto new_data = std::make_shared<base::data_t>( bytes_transfered );
								resp.read( new_data->data( ), static_cast<std::streamsize>(bytes_transfered) );
								read_buffer->consume( bytes_transfered );
								if( 0 < self->emitter( )->listener_count( "data_received" ) ) {

									{
										// Handle when the emitter comes after the data starts pouring in.  This might be best placed in newEvent
										// have not decided
										if( !response_buffers.empty( ) ) {
											auto buff = std::make_shared<base::data_t>( response_buffers.cbegin( ), response_buffers.cend( ) );
											self->m_response_buffers.resize( 0 );
											self->emit_data_received( buff, false );
										}
									}
									bool end_of_file = err && 2 == err.value( );

									self->emit_data_received( new_data, end_of_file );
								} else {	// Queue up for a													
									self->m_response_buffers.insert( self->m_response_buffers.cend( ), new_data->cbegin( ), new_data->cend( ) );
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
						} );
					}

					void NetSocketStreamImpl::handle_write( std::weak_ptr<NetSocketStreamImpl> obj, write_buffer buff, boost::system::error_code const & err, size_t bytes_transfered ) { // TODO see if we need buff, maybe lifetime issue
						run_if_valid( obj, "Exception while handling write", "NetSocketStreamImpl::handle_write", [&]( std::shared_ptr<NetSocketStreamImpl>& self ) {
							self->m_bytes_written += bytes_transfered;
							if( !err ) {
								self->emit_write_completion( );
							} else {
								self->emit_error( err, "NetSocket::handle_write" );
							}
							if( self->dec_outstanding_writes( ) ) {
								self->emit_all_writes_completed( );
							}
						} );
					}


					void NetSocketStreamImpl::emit_connect( ) {
						emitter( )->emit( "connect" );
					}

					void NetSocketStreamImpl::emit_timeout( ) {
						emitter( )->emit( "timeout" );
					}

					void NetSocketStreamImpl::write_async( write_buffer buff ) {
						if( m_state.closed || m_state.end ) {
							throw std::runtime_error( "Attempt to use a closed NetSocketStreamImplImpl" );
						}
						m_bytes_written += buff.size( );

						auto self = get_ptr( );
						auto handler = [self, buff]( boost::system::error_code const & err, size_t bytes_transfered ) mutable {
							self->handle_write( self, buff, err, bytes_transfered );
						};

						inc_outstanding_writes( );
						boost::asio::async_write( m_socket, buff.asio_buff( ), handler );
					}

					NetSocketStreamImpl&  NetSocketStreamImpl::read_async( std::shared_ptr<boost::asio::streambuf> read_buffer ) {
						if( m_state.closed ) {
							return *this;
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
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::on_connected( std::function<void( )> listener ) {
						emitter( )->add_listener( "connect", listener );
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::on_next_connected( std::function<void( )> listener ) {
						emitter( )->add_listener( "connect", listener, true );
						return *this;
					}


					NetSocketStreamImpl&  NetSocketStreamImpl::connect( std::string host, uint16_t port ) {
						tcp::resolver resolver( base::ServiceHandle::get( ) );

						auto self = get_ptr( );
						auto handler = [self]( boost::system::error_code const & err, tcp::resolver::iterator it ) {
							self->handle_connect( self, err, it );
						};

						boost::asio::async_connect( m_socket, resolver.resolve( { host, boost::lexical_cast<std::string>(port) } ), handler );
						return *this;
					}

					NetSocketStreamImpl&  NetSocketStreamImpl::connect( std::string path ) { throw std::runtime_error( "Method not implemented" ); }

					std::size_t& NetSocketStreamImpl::buffer_size( ) { throw std::runtime_error( "Method not implemented" ); }

					bool NetSocketStreamImpl::is_open( ) const {
						return m_socket.is_open( );
					}

					boost::asio::ip::tcp::socket & NetSocketStreamImpl::socket( ) {
						return m_socket;
					}

					NetSocketStreamImpl&  NetSocketStreamImpl::write_async( base::data_t const & chunk ) {
						write_async( write_buffer( chunk ) );
						return *this;
					}

					NetSocketStreamImpl&  NetSocketStreamImpl::write_async( boost::string_ref chunk, base::Encoding const & ) {
						write_async( write_buffer( chunk ) );
						return *this;
					}

					NetSocketStreamImpl&  NetSocketStreamImpl::end( ) {
						m_state.end = true;
						try {
							m_socket.shutdown( boost::asio::ip::tcp::socket::shutdown_send );
						} catch( ... ) {
							emit_error( std::current_exception( ), "Error calling shutdown on socket", "NetSocketStreamImplImpl::end( )" );
						}
						return *this;
					}

					NetSocketStreamImpl&  NetSocketStreamImpl::end( base::data_t const & chunk ) {
						write_async( chunk );
						end( );
						return *this;
					}

					NetSocketStreamImpl&  NetSocketStreamImpl::end( boost::string_ref chunk, base::Encoding const & encoding ) {
						write_async( chunk, encoding );
						end( );
						return *this;
					}

					void NetSocketStreamImpl::close( bool emit_cb ) {
						m_state.closed = true;
						m_state.end = true;
						try {
							m_socket.shutdown( boost::asio::ip::tcp::socket::shutdown_both );
							m_socket.close( );
						} catch( ... ) {
							//emit_error( std::current_exception( ), "Error calling shutdown on socket", "NetSocketStreamImplImpl::close( )" );
						}
						if( emit_cb ) {
							emit_closed( );
						}
					}

					void NetSocketStreamImpl::cancel( ) {
						m_socket.cancel( );
					}

					NetSocketStreamImpl&  NetSocketStreamImpl::set_timeout( int32_t ) { throw std::runtime_error( "Method not implemented" ); }

					NetSocketStreamImpl&  NetSocketStreamImpl::set_no_delay( bool ) { throw std::runtime_error( "Method not implemented" ); }

					NetSocketStreamImpl&  NetSocketStreamImpl::set_keep_alive( bool, int32_t ) { throw std::runtime_error( "Method not implemented" ); }

					std::string NetSocketStreamImpl::remote_address( ) const {
						return m_socket.remote_endpoint( ).address( ).to_string( );
					}

					std::string NetSocketStreamImpl::local_address( ) const {
						return m_socket.local_endpoint( ).address( ).to_string( );
					}

					uint16_t NetSocketStreamImpl::remote_port( ) const {
						return m_socket.remote_endpoint( ).port( );
					}

					uint16_t NetSocketStreamImpl::local_port( ) const {
						return m_socket.local_endpoint( ).port( );
					}

					std::size_t NetSocketStreamImpl::bytes_read( ) const {
						return m_bytes_read;
					}

					std::size_t NetSocketStreamImpl::bytes_written( ) const {
						return m_bytes_written;
					}

					// StreamReadable Interface
					base::data_t NetSocketStreamImpl::read( ) {
						return get_clear_buffer( m_response_buffers, m_response_buffers.size( ), 0 );
					}

					base::data_t  NetSocketStreamImpl::read( std::size_t ) { throw std::runtime_error( "Method not implemented" ); }


					bool NetSocketStreamImpl::is_closed( ) const {
						return m_state.closed;
					}

					bool NetSocketStreamImpl::can_write( ) const {
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
				}	// namespace impl

				NetSocketStream create_net_socket_stream( base::EventEmitter emitter ) {
					return NetSocketStream( new impl::NetSocketStreamImpl( emitter ) );
				}

				NetSocketStream create_net_socket_stream( boost::asio::io_service& io_service, std::size_t max_read_size, base::EventEmitter emitter ) {
					return NetSocketStream( new impl::NetSocketStreamImpl( io_service, max_read_size, emitter ) );
				}


			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
