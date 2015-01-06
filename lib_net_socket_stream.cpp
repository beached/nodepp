//	The MIT License (MIT)
//	
//	Copyright (c) 2014-2015 Darrell Wright
//	
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//	
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//	
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
//
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <condition_variable>
#include <cstdint>
#include <thread>
#include <string>

#include "base_enoding.h"
#include "base_error.h"
#include "base_event_emitter.h"
#include "base_service_handle.h"
#include "base_stream.h"
#include "base_types.h"
#include "base_write_buffer.h"
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
						m_socket( std::make_shared<boost::asio::ip::tcp::socket>( base::ServiceHandle::get( ) ) ),
						m_emitter( std::move( emitter ) ),
						m_state( ),
						m_read_options( ),
						m_pending_writes( new daw::thread::Semaphore<int>( ) ),
						m_response_buffers( ),
						m_bytes_read( 0 ),
						m_bytes_written( 0 ) { }

					NetSocketStreamImpl::NetSocketStreamImpl( boost::asio::io_service& io_service, std::size_t max_read_size, base::EventEmitter emitter ) :
						m_socket( std::make_shared<boost::asio::ip::tcp::socket>( io_service ) ),
						m_emitter( std::move( emitter ) ),
						m_state( ),
						m_read_options( max_read_size ),
						m_pending_writes( new daw::thread::Semaphore<int>( ) ),
						m_response_buffers( ),
						m_bytes_read( 0 ),
						m_bytes_written( 0 ) { }

					NetSocketStreamImpl::NetSocketStreamImpl( NetSocketStreamImpl && other ) :
						m_socket( std::move( other.m_socket ) ),
						m_emitter( std::move( other.m_emitter ) ),
						m_state( std::move( other.m_state ) ),
						m_read_options( std::move( other.m_read_options ) ),
						m_pending_writes( std::move( other.m_pending_writes ) ),
						m_response_buffers( std::move( other.m_response_buffers ) ),
						m_bytes_read( std::move( other.m_bytes_read ) ),
						m_bytes_written( std::move( other.m_bytes_written ) ) { }

					NetSocketStreamImpl& NetSocketStreamImpl::operator=(NetSocketStreamImpl && rhs) {
						if( this != &rhs ) {
							m_socket = std::move( rhs.m_socket );
							m_emitter = std::move( rhs.m_emitter );
							m_state = std::move( rhs.m_state );
							m_read_options = std::move( rhs.m_read_options );
							m_pending_writes = std::move( rhs.m_pending_writes );
							m_response_buffers = std::move( rhs.m_response_buffers );
							m_bytes_read = std::move( rhs.m_bytes_read );
							m_bytes_written = std::move( rhs.m_bytes_written );
						}
						return *this;
					}

					NetSocketStreamImpl::~NetSocketStreamImpl( ) {
						try {
							if( m_socket && m_socket->is_open( ) ) {
								boost::system::error_code ec;
								m_socket->shutdown( boost::asio::socket_base::shutdown_both, ec );
								m_socket->close( ec );
							}
						} catch( ... ) {
							// Do nothing, we don't usually care.  It's gone, move on
						}
					}

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
						m_read_options.read_predicate = daw::make_unique<NetSocketStreamImpl::match_function_t>( std::move( read_predicate ) );
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
					
					void NetSocketStreamImpl::handle_connect( std::weak_ptr<NetSocketStreamImpl> obj, boost::system::error_code const & err, tcp::resolver::iterator ) {
						run_if_valid( obj, "Exception while connecting", "NetSocketStreamImpl::handle_connect", [&err]( NetSocketStream self ) {
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

					void NetSocketStreamImpl::handle_read( std::weak_ptr<NetSocketStreamImpl> obj, std::shared_ptr<boost::asio::streambuf> read_buffer, boost::system::error_code const & err, std::size_t const & bytes_transfered ) {
						run_if_valid( obj, "Exception while handling read", "NetSocketStreamImpl::handle_read", [&]( NetSocketStream self ) {
							auto& response_buffers = self->m_response_buffers;

							read_buffer->commit( bytes_transfered );
							if( 0 < bytes_transfered ) {
								std::istream resp( read_buffer.get( ) );
								auto new_data = std::make_shared<base::data_t>( bytes_transfered );
								resp.read( new_data->data( ), static_cast<std::streamsize>(bytes_transfered) );
								read_buffer->consume( bytes_transfered );
								if( 0 < self->emitter( )->listener_count( "data_received" ) ) {
									// Handle when the emitter comes after the data starts pouring in.  This might be best placed in newEvent
									// have not decided
									if( !response_buffers.empty( ) ) {
										auto buff = std::make_shared<base::data_t>( response_buffers.cbegin( ), response_buffers.cend( ) );
										self->m_response_buffers.resize( 0 );
										self->emit_data_received( buff, false );
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

					void NetSocketStreamImpl::handle_write( std::weak_ptr<daw::thread::Semaphore<int>> outstanding_writes, std::weak_ptr<NetSocketStreamImpl> obj, base::write_buffer buff, boost::system::error_code const & err, size_t const & bytes_transfered ) { // TODO see if we need buff, maybe lifetime issue
						run_if_valid( obj, "Exception while handling write", "NetSocketStreamImpl::handle_write", [&]( NetSocketStream self ) {
							self->m_bytes_written += bytes_transfered;
							if( !err ) {
								self->emit_write_completion( );
							} else {
								self->emit_error( err, "NetSocket::handle_write" );
							}
							if( self->m_pending_writes->dec_counter( ) ) {
								self->emit_all_writes_completed( );
							}
						} );
						if( obj.expired( ) ) {
							if( !outstanding_writes.expired( ) ) {
								outstanding_writes.lock( )->dec_counter( );								
							}
						}
					}


					void NetSocketStreamImpl::emit_connect( ) {
						emitter( )->emit( "connect" );
					}

					void NetSocketStreamImpl::emit_timeout( ) {
						emitter( )->emit( "timeout" );
					}

					void NetSocketStreamImpl::write_async( base::write_buffer buff ) {
						if( m_state.closed || m_state.end ) {
							throw std::runtime_error( "Attempt to use a closed NetSocketStreamImplImpl" );
						}
						m_bytes_written += buff.size( );

						auto obj = get_weak_ptr( );
						auto outstanding_writes = m_pending_writes->get_weak_ptr( );

						m_pending_writes->inc_counter( );
						boost::asio::async_write( *m_socket, buff.asio_buff( ), [outstanding_writes, obj, buff]( boost::system::error_code const & err, size_t bytes_transfered ) mutable {
							handle_write( outstanding_writes, obj, buff, err, bytes_transfered );
						} );
					}

					NetSocketStreamImpl&  NetSocketStreamImpl::read_async( std::shared_ptr<boost::asio::streambuf> read_buffer ) {
						try {
							if( m_state.closed ) {
								return *this;
							}
							if( !read_buffer ) {
								read_buffer = std::make_shared<boost::asio::streambuf>( m_read_options.max_read_size );
							}

							auto obj = get_weak_ptr( );
							auto handler = [obj, read_buffer]( boost::system::error_code const & err, std::size_t bytes_transfered ) mutable {
								handle_read( obj, read_buffer, err, bytes_transfered );
							};
							static boost::regex const dbl_newline( R"((?:\r\n|\n){2})" );

							switch( m_read_options.read_mode ) {
							case ReadUntil::next_byte:
								throw std::runtime_error( "Read Until mode not implemented" );
							case ReadUntil::buffer_full:
								boost::asio::async_read( *m_socket, *read_buffer, handler );
								break;
							case ReadUntil::newline:
								boost::asio::async_read_until( *m_socket, *read_buffer, "\n", handler );
								break;
							case ReadUntil::double_newline:
								boost::asio::async_read_until( *m_socket, *read_buffer, dbl_newline, handler );
								break;
							case ReadUntil::predicate:
								boost::asio::async_read_until( *m_socket, *read_buffer, *m_read_options.read_predicate, handler );
								break;
							case ReadUntil::values:
								boost::asio::async_read_until( *m_socket, *read_buffer, m_read_options.read_until_values, handler );
								break;
							case ReadUntil::regex:
								boost::asio::async_read_until( *m_socket, *read_buffer, boost::regex( m_read_options.read_until_values ), handler );
								break;

							default:
								throw std::runtime_error( "read until method not implemented" );
							}
						} catch( ... ) {
							emit_error( std::current_exception( ), "Exception starting async read", "NetSocketStreamImpl::read_async" );
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


					NetSocketStreamImpl&  NetSocketStreamImpl::connect( boost::string_ref host, uint16_t port ) {
						tcp::resolver resolver( base::ServiceHandle::get( ) );

						auto obj = get_weak_ptr( );
						boost::asio::async_connect( *m_socket, resolver.resolve( { host.to_string(), std::to_string(port) } ), [obj]( boost::system::error_code const & err, tcp::resolver::iterator it ) {
							handle_connect( obj, err, it );
						} );
						return *this;
					}

					std::size_t& NetSocketStreamImpl::buffer_size( ) { throw std::runtime_error( "Method not implemented" ); }

					bool NetSocketStreamImpl::is_open( ) const {
						return m_socket->is_open( );
					}

					boost::asio::ip::tcp::socket & NetSocketStreamImpl::socket( ) {
						return *m_socket;
					}

					NetSocketStreamImpl&  NetSocketStreamImpl::write_async( base::data_t const & chunk ) {
						write_async( base::write_buffer( chunk ) );
						return *this;
					}

					NetSocketStreamImpl&  NetSocketStreamImpl::write_async( boost::string_ref chunk, base::Encoding const & ) {
						write_async( base::write_buffer( chunk ) );
						return *this;
					}

					NetSocketStreamImpl&  NetSocketStreamImpl::end( ) {
						m_state.end = true;
						try {
							m_socket->shutdown( boost::asio::ip::tcp::socket::shutdown_send );
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
							if( m_socket && m_socket->is_open( ) ) {
								boost::system::error_code err;
								m_socket->shutdown( boost::asio::ip::tcp::socket::shutdown_both, err );
								if( emit_cb && err && err.value() != 107 ) {	// Already shutdown is ignored
									emit_error( err, "NetSocketStreamImpl::close#shutdown" );
								}
								if( !m_state.closed ) {
									err = boost::system::error_code( );
									m_socket->close( err );
									if( emit_cb && err ) {
										emit_error( err, "NetSocketStreamImpl::close#close" );
									}
								}
								m_socket.reset( );
							}
							
						} catch( ... ) {
							//emit_error( std::current_exception( ), "Error calling shutdown on socket", "NetSocketStreamImplImpl::close( )" );
						}
						if( emit_cb ) {
							emit_closed( );
						}
					}

					void NetSocketStreamImpl::cancel( ) {
						m_socket->cancel( );
					}

					NetSocketStreamImpl&  NetSocketStreamImpl::set_timeout( int32_t ) { throw std::runtime_error( "Method not implemented" ); }

					NetSocketStreamImpl&  NetSocketStreamImpl::set_no_delay( bool ) { throw std::runtime_error( "Method not implemented" ); }

					NetSocketStreamImpl&  NetSocketStreamImpl::set_keep_alive( bool, int32_t ) { throw std::runtime_error( "Method not implemented" ); }

					std::string NetSocketStreamImpl::remote_address( ) const {
						return m_socket->remote_endpoint( ).address( ).to_string( );
					}

					std::string NetSocketStreamImpl::local_address( ) const {
						return m_socket->local_endpoint( ).address( ).to_string( );
					}

					uint16_t NetSocketStreamImpl::remote_port( ) const {
						return m_socket->remote_endpoint( ).port( );
					}

					uint16_t NetSocketStreamImpl::local_port( ) const {
						return m_socket->local_endpoint( ).port( );
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
