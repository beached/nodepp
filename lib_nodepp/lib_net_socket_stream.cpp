// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <condition_variable>
#include <cstdint>
#include <thread>
#include <string>
#include <boost/variant/static_visitor.hpp>
#include "base_enoding.h"
#include "base_error.h"
#include "base_event_emitter.h"
#include "base_service_handle.h"
#include "base_stream.h"
#include "base_types.h"
#include "base_write_buffer.h"
#include "lib_net_dns.h"

#include <daw/make_unique.h>

#include "lib_net_socket_stream.h"
#include "base_selfdestruct.h"

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
					base::data_t get_clear_buffer( base::data_t& original_buffer, size_t num_items, size_t new_size = 1024 ) {
						base::data_t new_buffer( new_size, 0 );
						using std::swap;
						swap( new_buffer, original_buffer );
						new_buffer.resize( num_items );
						return new_buffer;
					}

					NetSocketStreamImpl::NetSocketStreamImpl( std::shared_ptr<boost::asio::ssl::context> ctx, base::EventEmitter emitter ):
						daw::nodepp::base::SelfDestructing<NetSocketStreamImpl>( std::move( emitter ) ),
						m_socket( std::move( ctx ) ),
						m_state( ),
						m_read_options( ),
						m_pending_writes( new daw::thread::Semaphore<int>( ) ),
						m_response_buffers( ),
						m_bytes_read( 0 ),
						m_bytes_written( 0 ) { }

					NetSocketStreamImpl::~NetSocketStreamImpl( ) {
						try {
							if( m_socket && m_socket.is_open( ) ) {
								base::ErrorCode ec;
								m_socket.shutdown( boost::asio::socket_base::shutdown_both, ec );
								m_socket.close( ec );
							}
						} catch( ... ) {
							// Do nothing, we don't usually care.  It's gone, move on
						}
					}

					NetSocketStreamImpl& NetSocketStreamImpl::set_read_mode( NetSocketStreamReadMode mode ) {
						m_read_options.read_mode = mode;
						return *this;
					}

					NetSocketStreamReadMode const & NetSocketStreamImpl::current_read_mode( ) const {
						return m_read_options.read_mode;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::set_read_predicate( NetSocketStreamImpl::match_function_t read_predicate ) {
						m_read_options.read_predicate = daw::make_unique<NetSocketStreamImpl::match_function_t>( std::move( read_predicate ) );
						m_read_options.read_mode = NetSocketStreamReadMode::predicate;
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::clear_read_predicate( ) {
						if( NetSocketStreamReadMode::predicate == m_read_options.read_mode ) {
							m_read_options.read_mode = NetSocketStreamReadMode::newline;
						}
						m_read_options.read_until_values.clear( );
						m_read_options.read_predicate.reset( );
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::set_read_until_values( std::string values, bool is_regex ) {
						m_read_options.read_mode = is_regex ? NetSocketStreamReadMode::regex : NetSocketStreamReadMode::values;
						m_read_options.read_until_values = std::move( values );
						m_read_options.read_predicate.reset( );
						return *this;
					}

					void NetSocketStreamImpl::handle_connect( std::weak_ptr<NetSocketStreamImpl> obj, base::ErrorCode const & err, tcp::resolver::iterator ) {
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

					void NetSocketStreamImpl::handle_read( std::weak_ptr<NetSocketStreamImpl> obj, std::shared_ptr<daw::nodepp::base::stream::StreamBuf> read_buffer, base::ErrorCode const & err, std::size_t const & bytes_transfered ) {
						run_if_valid( obj, "Exception while handling read", "NetSocketStreamImpl::handle_read", [&]( NetSocketStream self ) {
							auto& response_buffers = self->m_response_buffers;

							read_buffer->commit( bytes_transfered );
							if( 0 <bytes_transfered ) {
								std::istream resp( read_buffer.get( ) );
								auto new_data = std::make_shared<base::data_t>( bytes_transfered, 0 );
								resp.read( new_data->data( ), static_cast<std::streamsize>(bytes_transfered) );
								read_buffer->consume( bytes_transfered );
								if( 0 <self->emitter( )->listener_count( "data_received" ) ) {
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

					void NetSocketStreamImpl::handle_write( std::weak_ptr<daw::thread::Semaphore<int>> outstanding_writes, std::weak_ptr<NetSocketStreamImpl> obj, base::write_buffer buff, base::ErrorCode const & err, size_t const & bytes_transfered ) { // TODO: see if we need buff, maybe lifetime issue
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
						this->emitter( )->emit( "connect" );
					}

					void NetSocketStreamImpl::emit_timeout( ) {
						this->emitter( )->emit( "timeout" );
					}

					void NetSocketStreamImpl::write_async( base::write_buffer buff ) {
						if( m_state.closed || m_state.end ) {
							throw std::runtime_error( "Attempt to use a closed NetSocketStreamImplImpl" );
						}
						m_bytes_written += buff.size( );

						auto obj = this->get_weak_ptr( );
						auto outstanding_writes = m_pending_writes->get_weak_ptr( );

						m_pending_writes->inc_counter( );
						m_socket.async_write( buff.asio_buff( ), [outstanding_writes, obj, buff]( base::ErrorCode const & err, size_t bytes_transfered ) mutable {
							handle_write( outstanding_writes, obj, buff, err, bytes_transfered );
						} );
					}

					NetSocketStreamImpl& NetSocketStreamImpl::read_async( std::shared_ptr<daw::nodepp::base::stream::StreamBuf> read_buffer ) {
						try {
							if( m_state.closed ) {
								return *this;
							}
							if( !read_buffer ) {
								read_buffer = std::make_shared<daw::nodepp::base::stream::StreamBuf>( m_read_options.max_read_size );
							}

							auto obj = this->get_weak_ptr( );
							auto handler = [obj, read_buffer]( base::ErrorCode const & err, std::size_t bytes_transfered ) mutable {
								handle_read( obj, read_buffer, err, bytes_transfered );
							};
							static boost::regex const dbl_newline( R"((?:\r\n|\n){2})" );

							switch( m_read_options.read_mode ) {
							case NetSocketStreamReadMode::next_byte:
								throw std::runtime_error( "Read Until mode not implemented" );
							case NetSocketStreamReadMode::buffer_full:
								m_socket.async_read( *read_buffer, handler );
								break;
							case NetSocketStreamReadMode::newline:
								m_socket.async_read_until( *read_buffer, "\n", handler );
								break;
							case NetSocketStreamReadMode::double_newline:
								m_socket.async_read_until( *read_buffer, dbl_newline, handler );
								break;
							case NetSocketStreamReadMode::predicate:
								m_socket.async_read_until( *read_buffer, *m_read_options.read_predicate, handler );
								break;
							case NetSocketStreamReadMode::values:
								m_socket.async_read_until( *read_buffer, m_read_options.read_until_values, handler );
								break;
							case NetSocketStreamReadMode::regex:
								m_socket.async_read_until( *read_buffer, boost::regex( m_read_options.read_until_values ), handler );
								break;

							default:
								throw std::runtime_error( "read until method not implemented" );
							}
						} catch( ... ) {
							this->emit_error( std::current_exception( ), "Exception starting async read", "NetSocketStreamImpl::read_async" );
						}
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::on_connected( std::function<void( NetSocketStream )> listener ) {
						this->emitter( )->add_listener( "connect", [obj = this->get_weak_ptr( ), cb=listener]( ) {
							if( auto self = obj.lock( ) ) {
								cb( self );
							}
						} );
						return *this;
					}

					NetSocketStreamImpl& NetSocketStreamImpl::on_next_connected( std::function<void( NetSocketStream )> listener ) {
						this->emitter( )->add_listener( "connect", [obj = this->get_weak_ptr( ), cb = listener]( ) {
							if( auto self = obj.lock( ) ) {
								cb( self );
							}
						}, true );
						return *this;
					}

					NetSocketStreamImpl&  NetSocketStreamImpl::connect( boost::string_ref host, uint16_t port ) {
						tcp::resolver resolver( base::ServiceHandle::get( ) );

						auto obj = this->get_weak_ptr( );
						m_socket.async_connect( resolver.resolve( { host.to_string( ), std::to_string( port ) } ), [obj]( base::ErrorCode const & err, tcp::resolver::iterator it ) {
							handle_connect( obj, err, it );
						} );
						return *this;
					}

					std::size_t& NetSocketStreamImpl::buffer_size( ) { throw std::runtime_error( "Method not implemented" ); }

					bool NetSocketStreamImpl::is_open( ) const {
						return m_socket.is_open( );
					}

					daw::nodepp::lib::net::impl::BoostSocket NetSocketStreamImpl::socket( ) {
						return m_socket;
					}

					NetSocketStreamImpl&  NetSocketStreamImpl::write_async( base::data_t const & chunk ) {
						this->write_async( base::write_buffer( chunk ) );
						return *this;
					}

					NetSocketStreamImpl&  NetSocketStreamImpl::write_async( boost::string_ref chunk, base::Encoding const & ) {
						this->write_async( base::write_buffer( chunk ) );
						return *this;
					}

					NetSocketStreamImpl&  NetSocketStreamImpl::end( ) {
						m_state.end = true;
						try {
							m_socket.shutdown( boost::asio::ip::tcp::socket::shutdown_send );
						} catch( ... ) {
							this->emit_error( std::current_exception( ), "Error calling shutdown on socket", "NetSocketStreamImplImpl::end( )" );
						}
						return *this;
					}

					NetSocketStreamImpl&  NetSocketStreamImpl::end( base::data_t const & chunk ) {
						this->write_async( chunk );
						this->end( );
						return *this;
					}

					NetSocketStreamImpl&  NetSocketStreamImpl::end( boost::string_ref chunk, base::Encoding const & encoding ) {
						this->write_async( chunk, encoding );
						this->end( );
						return *this;
					}

					void NetSocketStreamImpl::close( bool emit_cb ) {
						m_state.closed = true;
						m_state.end = true;
						try {
							if( m_socket && m_socket.is_open( ) ) {
								base::ErrorCode err;

								m_socket.shutdown( boost::asio::ip::tcp::socket::shutdown_both, err );
								if( emit_cb && err && err.value( ) != 107 ) {	// Already shutdown is ignored
									emit_error( err, "NetSocketStreamImpl::close#shutdown" );
								}
								if( !m_state.closed ) {
									err = base::ErrorCode( );
									m_socket.close( err );
									if( emit_cb && err ) {
										emit_error( err, "NetSocketStreamImpl::close#close" );
									}
								}
								m_socket.reset_socket( );
							}
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

					base::data_t NetSocketStreamImpl::read( std::size_t ) { throw std::runtime_error( "Method not implemented" ); }

					bool NetSocketStreamImpl::is_closed( ) const {
						return m_state.closed;
					}

					bool NetSocketStreamImpl::can_write( ) const {
						return !m_state.end;
					}
				}	// namespace impl

				NetSocketStream create_net_socket_stream( daw::nodepp::base::EventEmitter emitter ) {
					auto tmp = new impl::NetSocketStreamImpl( std::shared_ptr<boost::asio::ssl::context>( nullptr ), std::move( emitter ) );
					auto result = NetSocketStream( tmp );
					result->arm( "close" );
					return result;
				}

				NetSocketStream create_net_socket_stream( std::shared_ptr<boost::asio::ssl::context> context, daw::nodepp::base::EventEmitter emitter ) {
					auto tmp = new impl::NetSocketStreamImpl( std::move( context ), std::move( emitter ) );
					auto result = NetSocketStream( tmp );
					result->arm( "close" );
					return result;
				}

				NetSocketStream create_net_socket_stream( boost::asio::ssl::context::method method, daw::nodepp::base::EventEmitter emitter ) {
					auto tmp = new impl::NetSocketStreamImpl( std::make_shared<boost::asio::ssl::context>( method ), std::move( emitter ) );
					auto result = NetSocketStream( tmp );
					result->arm( "close" );
					return result;
				}

// 				daw::nodepp::lib::net::NetSocketStream& operator<<( daw::nodepp::lib::net::NetSocketStream socket, boost::string_ref message ) {
// 					if( socket ) {
// 						socket->write_async( message );
// 					} else {
// 						throw std::runtime_error( "Attempt to use a null NetSocketStream" );
// 					}
// 					return socket;
// 				}
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw

daw::nodepp::lib::net::NetSocketStream& operator<<( daw::nodepp::lib::net::NetSocketStream & socket, boost::string_ref message ) {
	if( socket ) {
		socket->write_async( message );
	} else {
		throw std::runtime_error( "Attempt to use a null NetSocketStream" );
	}
	return socket;
}

