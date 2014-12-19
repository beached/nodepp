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
#include "lib_net_socket_stream_impl.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				using namespace daw::nodepp;
				using namespace boost::asio::ip;

				namespace impl {
					using namespace daw::nodepp;
					using namespace boost::asio::ip;

					write_buffer::write_buffer( base::data_t const & source ) : buff( std::make_shared<base::data_t>( source ) ) { }

					write_buffer::write_buffer( std::string const & source ) : buff( std::make_shared<base::data_t>( source.begin( ), source.end( ) ) ) { }

					std::size_t write_buffer::size( ) const {
						return buff->size( );
					}

					write_buffer::data_type write_buffer::data( ) const {
						return buff->data( );
					}

					boost::asio::mutable_buffers_1 write_buffer::asio_buff( ) const {
						return boost::asio::buffer( data( ), size( ) );
					}

					NetSocketStreamImpl::NetSocketStreamImpl( ) : base::stream::Stream( ),
						m_socket( base::ServiceHandle::get( ) ),
						m_max_read_size( 8192 ),
						m_response_buffers( ),
						m_bytes_read( 0 ),
						m_bytes_written( 0 ),
						m_read_mode( NetSocketStream::ReadUntil::newline ),
						m_read_predicate( ),
						m_outstanding_writes( 0 ),
						m_closed( false ),
						m_end( false ),
						m_read_until_values( ) { 
						std::cerr << "NetSocketStreamImpl( )\n";
					}

					NetSocketStreamImpl::NetSocketStreamImpl( boost::asio::io_service& io_service, std::size_t max_read_size ) : base::stream::Stream( ),
						m_socket( io_service ),
						m_max_read_size( max_read_size ),
						m_response_buffers( ),
						m_bytes_read( 0 ),
						m_bytes_written( 0 ),
						m_read_mode( NetSocketStream::ReadUntil::newline ),
						m_read_predicate( ),
						m_outstanding_writes( 0 ),
						m_closed( false ),
						m_end( false ),
						m_read_until_values( ) {
						std::cerr << "NetSocketStreamImpl( boost::asio::io_service& io_service, std::size_t max_read_size )\n";
					}

					NetSocketStreamImpl::~NetSocketStreamImpl( ) {
						try {
							if( m_socket.is_open( ) ) {
								m_socket.close( );
							}
						} catch( std::exception const & ) {
							// Do nothing, we don't usually care.  It's gone, move on
						}
						std::cerr << "~NetSocketStreamImpl( )\n";
					}

					namespace {

						void emit_error( NetSocketStreamImpl * const net_socket, boost::system::error_code const & err, std::string where ) {
							auto error = base::Error( err );
							error.add( "where", where );
							net_socket->emit( "error", error );
						}

						void emit_error( NetSocketStreamImpl * const net_socket, std::exception_ptr ex, std::string description, std::string where ) {
							auto error = base::Error( description, ex );
							error.add( "where", where );
							net_socket->emit( "error", error );
						}


						void emit_data( NetSocketStreamImpl* const net_socket, std::shared_ptr<base::data_t> buffer, bool end_of_file ) {
							net_socket->emit( "data", std::move( buffer ), end_of_file );
							if( end_of_file ) {
								net_socket->emit( "end" );
							}
						}

						void connect_handler( NetSocketStreamImpl* const net_socket, boost::system::error_code const & err, tcp::resolver::iterator it ) {
							if( !err ) {
								net_socket->emit( "connect" );
							} else {
								emit_error( net_socket, err, "NetSocket::connect" );
							}
						}
					}	// namespace anonymous

					void NetSocketStreamImpl::inc_outstanding_writes( ) {
						m_outstanding_writes++;
					}

					bool NetSocketStreamImpl::dec_outstanding_writes( ) {
						return 0 == --m_outstanding_writes;
					}

					std::vector<std::string> const & NetSocketStreamImpl::valid_events( ) const {
						static auto const result = [&]( ) {
							auto local = std::vector < std::string > { "connect", "timeout" };
							return base::impl::append_vector( local, base::stream::Stream::valid_events( ) );
						}();
						return result;
					}

					base::data_t get_clear_buffer( base::data_t& original_buffer, size_t num_items, size_t new_size = 1024 ) {
						base::data_t new_buffer( new_size, 0 );
						using std::swap;
						swap( new_buffer, original_buffer );
						new_buffer.resize( num_items );
						return new_buffer;
					}

					void NetSocketStreamImpl::handle_read( std::shared_ptr<boost::asio::streambuf> read_buffer, boost::system::error_code const & err, std::size_t bytes_transfered ) {

						read_buffer->commit( bytes_transfered );
						if( 0 < bytes_transfered ) {
							std::istream resp( read_buffer.get( ) );
							auto new_data = std::make_shared<base::data_t>( bytes_transfered );
							resp.read( new_data->data( ), static_cast<std::streamsize>(bytes_transfered) );
							read_buffer->consume( bytes_transfered );
							if( 0 < listener_count( "data" ) ) {

								{
									// Handle when the emitter comes after the data starts pouring in.  This might be best placed in newEvent
									// have not decided
									if( !m_response_buffers.empty( ) ) {
										auto buff = std::make_shared<base::data_t>( get_clear_buffer( m_response_buffers, m_response_buffers.size( ), 0 ) );
										emit_data( this, std::move( buff ), false );
									}
								}
								bool end_of_file = err && 2 == err.value( );

								emit_data( this, std::move( new_data ), end_of_file );
							} else {	// Queue up for a			
								daw::copy_vect_and_set( *new_data, m_response_buffers, bytes_transfered, static_cast<base::data_t::value_type>(0) );
							}
							m_bytes_read += bytes_transfered;
						}

						if( !err ) {
							if( !m_closed ) {
								read_async( read_buffer );
							}
						} else if( 2 != err.value( ) ) {
							emit_error( this, err, "NetSocket::read" );
						}
					}

					void NetSocketStreamImpl::read_async( std::shared_ptr<boost::asio::streambuf> read_buffer ) {
						if( m_closed ) {
							return;
						}
						if( !read_buffer ) {
							read_buffer = std::make_shared<boost::asio::streambuf>( m_max_read_size );
						}

						auto handler = [&, read_buffer]( boost::system::error_code const & err, std::size_t bytes_transfered ) mutable {
							handle_read( read_buffer, err, bytes_transfered );
						};

						switch( m_read_mode ) {
						case NetSocketStream::ReadUntil::next_byte:
							throw std::runtime_error( "Read Until mode not implemented" );
							//boost::asio::async_read( m_socket, m_response_buffer2, boost::asio::transfer_exactly( 1 ), handler );
						case NetSocketStream::ReadUntil::buffer_full:
							boost::asio::async_read( m_socket, *read_buffer, handler );
							break;
						case NetSocketStream::ReadUntil::newline:
							boost::asio::async_read_until( m_socket, *read_buffer, "\n", handler );
							break;
						case NetSocketStream::ReadUntil::predicate:
							boost::asio::async_read_until( m_socket, *read_buffer, *m_read_predicate, handler );
							break;
						case NetSocketStream::ReadUntil::values:
							boost::asio::async_read_until( m_socket, *read_buffer, m_read_until_values, handler );
							break;
						case NetSocketStream::ReadUntil::regex:
							boost::asio::async_read_until( m_socket, *read_buffer, boost::regex( m_read_until_values ), handler );
							break;

						default:
							throw std::runtime_error( "read until method not implemented" );
						}

					}

					NetSocketStreamImpl& NetSocketStreamImpl::set_read_mode( NetSocketStream::ReadUntil mode ) {
						m_read_mode = mode;
						return *this;
					}

					NetSocketStream::ReadUntil const& NetSocketStreamImpl::current_read_mode( ) const {
						return m_read_mode;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::set_read_predicate( NetSocketStream::match_function_t read_predicate ) {
						m_read_predicate = daw::make_unique<NetSocketStream::match_function_t>( read_predicate );
						m_read_mode = NetSocketStream::ReadUntil::predicate;
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::clear_read_predicate( ) {
						if( NetSocketStream::ReadUntil::predicate == m_read_mode ) {
							m_read_mode = NetSocketStream::ReadUntil::newline;
						}
						m_read_until_values = "";
						m_read_predicate.reset( );
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::set_read_until_values( std::string const & values, bool is_regex ) {
						m_read_mode = is_regex ? NetSocketStream::ReadUntil::regex : NetSocketStream::ReadUntil::values;
						m_read_until_values = values;
						m_read_predicate.reset( );
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::when_connected( std::function<void( )> listener ) {
						add_listener( "connect", listener );
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::when_error( std::function<void( base::Error )> listener ) {
						add_listener( "error", listener );
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::when_data_recv( std::function<void( std::shared_ptr<base::data_t>, bool )> listener ) {
						add_listener( "data", listener );
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::when_eof( std::function<void( )> listener ) {
						add_listener( "end", listener );
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::when_closed( std::function<void( )> listener ) {
						add_listener( "close", listener );
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::when_a_write_completes( std::function<void( )> listener ) {
						add_listener( "drain", listener );
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::when_all_writes_complete( std::function<void( )> listener ) {
						add_listener( "finish", listener );
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::when_next_connected( std::function<void( )> listener ) {
						add_listener( "connect", listener, true );
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::when_next_error( std::function<void( base::Error )> listener ) {
						add_listener( "error", listener, true );
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::when_next_data_recv( std::function<void( std::shared_ptr<base::data_t>, bool )> listener ) {
						add_listener( "data", listener, true );
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::when_next_eof( std::function<void( )> listener ) {
						add_listener( "end", listener, true );
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::on_closed( std::function<void( )> listener ) {
						add_listener( "close", listener, true );
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::when_next_all_writes_complete( std::function<void( )> listener ) {
						add_listener( "finish", listener, true );
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::when_next_write_completes( std::function<void( )> listener ) {
						add_listener( "drain", listener, true );
						return *this;
					}


					NetSocketStreamImpl& NetSocketStreamImpl::connect( std::string host, uint16_t port ) {
						tcp::resolver resolver( base::ServiceHandle::get( ) );

						auto handler = [&]( boost::system::error_code const & err, tcp::resolver::iterator it ) {
							connect_handler( this, err, it );
						};

						boost::asio::async_connect( m_socket, resolver.resolve( { host, boost::lexical_cast<std::string>(port) } ), handler );
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::connect( std::string path ) { throw std::runtime_error( "Method not implemented" ); }

					std::size_t& NetSocketStreamImpl::buffer_size( ) { throw std::runtime_error( "Method not implemented" ); }

					std::size_t const & NetSocketStreamImpl::buffer_size( ) const { throw std::runtime_error( "Method not implemented" ); }


					bool NetSocketStreamImpl::is_open( ) const {
						return m_socket.is_open( );
					}

					void NetSocketStreamImpl::handle_write( write_buffer buff, boost::system::error_code const & err, size_t bytes_transfered ) { // TODO see if we need buff, maybe lifetime issue
						m_bytes_written += bytes_transfered;
						if( !err ) {
							emit( "drain" );
						} else {
							emit_error( this, err, "NetSocket::write" );
						}
						if( dec_outstanding_writes( ) ) {
							emit( "finish" );
						}
					}

					boost::asio::ip::tcp::socket & NetSocketStreamImpl::socket( ) {
						return m_socket;
					}

					boost::asio::ip::tcp::socket const & NetSocketStreamImpl::socket( ) const {
						return m_socket;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::write( write_buffer buff ) {
						if( m_closed || m_end ) {
							throw std::runtime_error( "Attempt to use a closed NetSocketStreamImpl" );
						}
						m_bytes_written += buff.size( );

						auto handler = [&, buff]( boost::system::error_code const & err, size_t bytes_transfered ) mutable {
							handle_write( buff, err, bytes_transfered );
						};

						inc_outstanding_writes( );
						boost::asio::async_write( m_socket, buff.asio_buff( ), handler );
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::write( base::data_t const & chunk ) {
						return write( write_buffer( chunk ) );
					}

					NetSocketStreamImpl& NetSocketStreamImpl::write( std::string const & chunk, base::Encoding const & ) {
						return write( write_buffer( chunk ) );
					}

					void NetSocketStreamImpl::end( ) {
						m_end = true;
						try {
							m_socket.shutdown( boost::asio::ip::tcp::socket::shutdown_send );
						} catch( ... ) {
							emit_error( this, std::current_exception( ), "Error calling shutdown on socket", "NetSocketStreamImpl::end( )" );
						}
					}

					void NetSocketStreamImpl::end( base::data_t const & chunk ) {
						write( chunk );
						end( );
					}

					void NetSocketStreamImpl::end( std::string const & chunk, base::Encoding const & encoding ) {
						write( chunk, encoding );
						end( );
					}

					void NetSocketStreamImpl::close( bool emit_cb ) {
						m_closed = true;
						m_end = true;
						try {
							m_socket.shutdown( boost::asio::ip::tcp::socket::shutdown_both );
						} catch( ... ) {
							emit_error( this, std::current_exception( ), "Error calling shutdown on socket", "NetSocketStreamImpl::close( )" );
						}
						if( emit_cb ) {
							emit( "close" );
						}
					}

					void NetSocketStreamImpl::cancel( ) {
						m_socket.cancel( );
					}

					NetSocketStreamImpl& NetSocketStreamImpl::set_timeout( int32_t ) { throw std::runtime_error( "Method not implemented" ); }

					NetSocketStreamImpl& NetSocketStreamImpl::set_no_delay( bool ) { throw std::runtime_error( "Method not implemented" ); }
					NetSocketStreamImpl& NetSocketStreamImpl::set_keep_alive( bool, int32_t ) { throw std::runtime_error( "Method not implemented" ); }

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
					NetSocketStreamImpl& NetSocketStreamImpl::set_encoding( base::Encoding const & ) { throw std::runtime_error( "Method not implemented" ); }


					NetSocketStreamImpl& NetSocketStreamImpl::resume( ) { throw std::runtime_error( "Method not implemented" ); }
					NetSocketStreamImpl& NetSocketStreamImpl::pause( ) { throw std::runtime_error( "Method not implemented" ); }

					using base::stream::StreamWritable;

					StreamWritable& NetSocketStreamImpl::pipe( StreamWritable& ) { throw std::runtime_error( "Method not implemented" ); }
					StreamWritable& NetSocketStreamImpl::pipe( StreamWritable&, base::options_t ) { throw std::runtime_error( "Method not implemented" ); }
					NetSocketStreamImpl& NetSocketStreamImpl::unpipe( StreamWritable& ) { throw std::runtime_error( "Method not implemented" ); }
					NetSocketStreamImpl& NetSocketStreamImpl::unshift( base::data_t const & ) { throw std::runtime_error( "Method not implemented" ); }


					NetSocketStream& operator<<(NetSocketStream& net_socket, std::string const & value) {
						return net_socket.write( value );
					}

					NetSocketStream& operator<<(NetSocketStream& net_socket, base::data_t const & value) {
						return net_socket.write( value );
					}

					bool NetSocketStreamImpl::is_closed( ) const {
						return m_closed;
					}

					bool NetSocketStreamImpl::can_write( ) const {
						return !m_end;
					}
				} // namespace impl

				NetSocketStream::NetSocketStream( ) : m_impl( std::make_shared<impl::NetSocketStreamImpl>( ) ) { }

				NetSocketStream::NetSocketStream( boost::asio::io_service& io_service, size_t max_read_size ) : m_impl( std::make_shared<impl::NetSocketStreamImpl>( io_service, max_read_size ) ) { }

				NetSocketStream::NetSocketStream( NetSocketStream const & rhs ): m_impl( rhs.m_impl ) { }
				
				NetSocketStream::NetSocketStream( NetSocketStream && other ): m_impl( std::move( other.m_impl ) ) { }
				
				NetSocketStream& NetSocketStream::operator=( NetSocketStream rhs ) {
					m_impl = std::move( m_impl );
					return *this;
				}

				boost::asio::ip::tcp::socket & NetSocketStream::socket( ) { return m_impl->socket( ); }
				boost::asio::ip::tcp::socket const & NetSocketStream::socket( ) const { return m_impl->socket( ); }

				std::vector<std::string> const & NetSocketStream::valid_events( ) const {
					return m_impl->valid_events( );
				}

				NetSocketStream& NetSocketStream::set_read_mode( NetSocketStream::ReadUntil mode ) {
					m_impl->set_read_mode( mode );
					return *this;
				}

				NetSocketStream::ReadUntil const& NetSocketStream::current_read_mode( ) const {
					return m_impl->current_read_mode( );
				}

				NetSocketStream& NetSocketStream::set_read_predicate( std::function < std::pair<match_iterator_t, bool>( match_iterator_t begin, match_iterator_t end ) > match_function ) {
					m_impl->set_read_predicate( match_function );
					return *this;
				}

				NetSocketStream& NetSocketStream::clear_read_predicate( ) {
					m_impl->clear_read_predicate( );
					return *this;
				}

				NetSocketStream& NetSocketStream::set_read_until_values( std::string const & values, bool is_regex ) {
					m_impl->set_read_until_values( values, is_regex );
					return *this;
				}


				NetSocketStream& NetSocketStream::connect( std::string host, uint16_t port ) {
					m_impl->connect( host, port );
					return *this;
				}
				
				NetSocketStream& NetSocketStream::connect( std::string path ) {
					m_impl->connect( path );
					return *this;
				}

				size_t& NetSocketStream::buffer_size( ) { return m_impl->buffer_size( ); }
				size_t const & NetSocketStream::buffer_size( ) const { return m_impl->buffer_size( ); }

				NetSocketStream& NetSocketStream::set_timeout( int32_t value ) {
					m_impl->set_timeout( value );
					return *this;
				}

				NetSocketStream& NetSocketStream::set_no_delay( bool noDelay ) {
					m_impl->set_no_delay( noDelay );
					return *this;
				}
				
				NetSocketStream& NetSocketStream::set_keep_alive( bool keep_alive, int32_t initial_delay ) {
					m_impl->set_keep_alive( keep_alive, initial_delay );
					return *this;
				}

				std::string NetSocketStream::remote_address( ) const { return m_impl->remote_address( ); }
				std::string NetSocketStream::local_address( ) const { return m_impl->local_address( ); }
				uint16_t NetSocketStream::remote_port( ) const { return m_impl->remote_port( ); }
				uint16_t NetSocketStream::local_port( ) const { return m_impl->local_port( ); }

				size_t NetSocketStream::bytes_read( ) const { return m_impl->bytes_read( ); }
				size_t NetSocketStream::bytes_written( ) const { return m_impl->bytes_written( ); }

				bool NetSocketStream::is_open( ) const { return m_impl->is_open( ); }

				void NetSocketStream::read_async( ) { m_impl->read_async( ); }

				// StreamReadable Interface
				base::data_t NetSocketStream::read( ) { return m_impl->read( ); }

				base::data_t NetSocketStream::read( size_t bytes ) { return m_impl->read( bytes ); }

				NetSocketStream& NetSocketStream::set_encoding( base::Encoding const & encoding ) {
					m_impl->set_encoding( encoding );
					return *this;
				}
				
				NetSocketStream& NetSocketStream::resume( ) {
					m_impl->resume( );
					return *this;
				}
				NetSocketStream& NetSocketStream::pause( ) {
					m_impl->pause( );
					return *this;
				}
				
				base::stream::StreamWritable& NetSocketStream::pipe( base::stream::StreamWritable& destination ) {
					m_impl->pipe( destination );
					return *this;
				}
				
				base::stream::StreamWritable& NetSocketStream::pipe( base::stream::StreamWritable& destination, base::options_t options ) {
					m_impl->pipe( destination, options );
					return *this;
				}

				NetSocketStream& NetSocketStream::unpipe( StreamWritable& destination ) {
					m_impl->unpipe( destination );
					return *this;
				}

				NetSocketStream& NetSocketStream::unshift( base::data_t const & chunk ) { 
					m_impl->unshift( chunk );
					return *this; 
				}

				// StreamWritable Interface
				NetSocketStream& NetSocketStream::write( base::data_t const & chunk ) { 
					m_impl->write( chunk );
					return *this; 
				
				}

				NetSocketStream& NetSocketStream::write( std::string const & chunk, base::Encoding const & encoding ) {
					m_impl->write( chunk, encoding );
					return *this; 
				}

				void NetSocketStream::end( ) { m_impl->end( ); }
				
				void NetSocketStream::end( base::data_t const & chunk ) { m_impl->end( chunk ); }

				void NetSocketStream::end( std::string const & chunk, base::Encoding const & encoding ) { m_impl->end( chunk, encoding ); }

				void NetSocketStream::close( bool emit_cb ) { m_impl->close( emit_cb ); }
				void NetSocketStream::cancel( ) { m_impl->cancel( ); }

				bool NetSocketStream::is_closed( ) const { return m_impl->is_closed( ); }
				bool NetSocketStream::can_write( ) const { return m_impl->can_write( ); }

				// Event callbacks

				NetSocketStream& NetSocketStream::when_connected( std::function<void( )> listener ) {
					m_impl->when_connected( listener );
					return *this;
				}

				NetSocketStream& NetSocketStream::when_error( std::function<void( base::Error )> listener ) { 
					m_impl->when_error( listener );
					return *this; 
				}

				// StreamReadable callbacks

				NetSocketStream& NetSocketStream::when_data_recv( std::function<void( std::shared_ptr<base::data_t>, bool )> listener ) { 
					m_impl->when_data_recv( listener );
					return *this; 
				}

				NetSocketStream& NetSocketStream::when_eof( std::function<void( )> listener ) { 
					m_impl->when_eof( listener );
					return *this; 
				}

				NetSocketStream& NetSocketStream::when_closed( std::function<void( )> listener ) { 
					m_impl->when_closed( listener );
					return *this; 
				}

				NetSocketStream& NetSocketStream::when_a_write_completes( std::function<void( )> listener ) { 
					m_impl->when_a_write_completes( listener );
					return *this; 
				}

				NetSocketStream& NetSocketStream::when_all_writes_complete( std::function<void( )> listener ) { 
					m_impl->when_all_writes_complete( listener );
					return *this; 
				}

				NetSocketStream& NetSocketStream::when_next_connected( std::function<void( )> listener ) {
					m_impl->when_next_connected( listener );
					return *this;
				}

				NetSocketStream& NetSocketStream::when_next_error( std::function<void( base::Error )> listener ) {
					m_impl->when_next_error( listener );
					return *this;
				}

				// StreamReadable callbacks

				NetSocketStream& NetSocketStream::when_next_data_recv( std::function<void( std::shared_ptr<base::data_t>, bool )> listener ) { 
					m_impl->when_next_data_recv( listener );
					return *this; 
				}

				NetSocketStream& NetSocketStream::when_next_eof( std::function<void( )> listener ) {
					m_impl->when_next_eof( listener );
						return *this;
				}

				NetSocketStream& NetSocketStream::on_closed( std::function<void( )> listener ) {
					m_impl->on_closed( listener );
					return *this;
				}

				NetSocketStream& NetSocketStream::when_next_all_writes_complete( std::function<void( )> listener ) {
					m_impl->when_next_all_writes_complete( listener );
					return *this;
				}

				NetSocketStream& NetSocketStream::when_next_write_completes( std::function<void( )> listener ) {
					m_impl->when_next_write_completes( listener );
					return *this;
				}


			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
