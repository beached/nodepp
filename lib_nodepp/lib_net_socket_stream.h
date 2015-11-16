// The MIT License (MIT)
//
// Copyright (c) 2014-2015 Darrell Wright
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

#pragma once

#include <boost/asio.hpp>
#include <boost/variant.hpp>
#include <boost/asio/ssl.hpp>
#include <cstdint>
#include <memory>
#include <string>

#include "base_enoding.h"
#include "base_error.h"
#include "base_service_handle.h"
#include "base_stream.h"
#include "base_types.h"
#include "base_write_buffer.h"
#include "daw_semaphore.h"
#include "base_selfdestruct.h"
#include "lib_net_dns.h"
#include "lib_net_socket_boost_socket.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				namespace impl {
					struct NetSocketStreamImpl;
				}

				using NetSocketStream = std::shared_ptr < impl::NetSocketStreamImpl >;

				NetSocketStream create_net_socket_stream( daw::nodepp::base::EventEmitter emitter = daw::nodepp::base::create_event_emitter( ) );
				NetSocketStream create_net_ssl_socket_stream( daw::nodepp::base::EventEmitter emitter = daw::nodepp::base::create_event_emitter( ) );

				enum class NetSocketStreamReadMode { newline, buffer_full, predicate, next_byte, regex, values, double_newline };

				namespace impl {
					struct NetSocketStreamImpl: public daw::nodepp::base::SelfDestructing<NetSocketStreamImpl>, public daw::nodepp::base::stream::StreamReadableEvents<NetSocketStreamImpl>, public daw::nodepp::base::stream::StreamWritableEvents < NetSocketStreamImpl > {
						using match_iterator_t = boost::asio::buffers_iterator <base::stream::StreamBuf::const_buffers_type >;
						using match_function_t = std::function < std::pair<match_iterator_t, bool>( match_iterator_t begin, match_iterator_t end ) >;
					private:
						RawSocket m_socket;
						boost::asio::ssl::context m_context;
						daw::nodepp::base::EventEmitter m_emitter;

						struct netsockstream_state_t {
							bool closed;
							bool end;
							netsockstream_state_t( ): closed( false ), end( false ) { }
							netsockstream_state_t( netsockstream_state_t const & ) = default;
							netsockstream_state_t& operator=( netsockstream_state_t const & ) = default;
							~netsockstream_state_t( ) = default;
						} m_state;

						struct netsockstream_readoptions_t {
							NetSocketStreamReadMode read_mode;
							size_t max_read_size;
							std::unique_ptr<NetSocketStreamImpl::match_function_t> read_predicate;
							std::string read_until_values;

							netsockstream_readoptions_t( ):
								read_mode( NetSocketStreamReadMode::newline ),
								max_read_size( 8192 ),
								read_predicate( ),
								read_until_values( ) { }

							netsockstream_readoptions_t( size_t max_read_size_ ):
								read_mode( NetSocketStreamReadMode::newline ),
								max_read_size( max_read_size_ ),
								read_predicate( ),
								read_until_values( ) { }

							netsockstream_readoptions_t( netsockstream_readoptions_t const & ) = delete;
							netsockstream_readoptions_t& operator=( netsockstream_readoptions_t const & ) = delete;

							inline netsockstream_readoptions_t( netsockstream_readoptions_t && other ):
								read_mode( std::move( other.read_mode ) ),
								max_read_size( std::move( other.max_read_size ) ),
								read_predicate( std::move( other.read_predicate ) ),
								read_until_values( ) { }

							inline netsockstream_readoptions_t& operator=( netsockstream_readoptions_t && rhs ) {
								if( this != &rhs ) {
									read_mode = std::move( rhs.read_mode );
									max_read_size = std::move( rhs.max_read_size );
									read_predicate = std::move( rhs.read_predicate );
								}
								return *this;
							}
							~netsockstream_readoptions_t( ) = default;
						} m_read_options;

						std::shared_ptr<daw::thread::Semaphore<int>> m_pending_writes;
						daw::nodepp::base::data_t m_response_buffers;
						std::size_t m_bytes_read;
						std::size_t m_bytes_written;

						NetSocketStreamImpl( daw::nodepp::base::EventEmitter emitter, bool use_ssl = false );

					public:
						friend NetSocketStream daw::nodepp::lib::net::create_net_socket_stream( daw::nodepp::base::EventEmitter emitter );
						friend NetSocketStream daw::nodepp::lib::net::create_net_ssl_socket_stream( daw::nodepp::base::EventEmitter emitter );

						NetSocketStreamImpl( NetSocketStreamImpl&& other ) = default;
						NetSocketStreamImpl& operator=( NetSocketStreamImpl&& rhs ) = default;
						~NetSocketStreamImpl( );

						NetSocketStreamImpl( NetSocketStreamImpl const & ) = delete;
						NetSocketStreamImpl& operator=( NetSocketStreamImpl const & ) = delete;

						daw::nodepp::base::EventEmitter& emitter( );

						NetSocketStreamImpl&  read_async( std::shared_ptr<daw::nodepp::base::stream::StreamBuf> read_buffer = nullptr );
						daw::nodepp::base::data_t read( );
						daw::nodepp::base::data_t read( std::size_t bytes );

						NetSocketStreamImpl& write_async( daw::nodepp::base::data_t const & chunk );
						NetSocketStreamImpl& write_async( boost::string_ref chunk, daw::nodepp::base::Encoding const & encoding = daw::nodepp::base::Encoding( ) );

						NetSocketStreamImpl& end( );
						NetSocketStreamImpl& end( daw::nodepp::base::data_t const & chunk );
						NetSocketStreamImpl& end( boost::string_ref chunk, daw::nodepp::base::Encoding const & encoding = daw::nodepp::base::Encoding( ) );

						NetSocketStreamImpl& connect( boost::string_ref host, uint16_t port );

						void close( bool emit_cb = true );
						void cancel( );

						bool is_open( ) const;
						bool is_closed( ) const;
						bool can_write( ) const;

						NetSocketStreamImpl& set_read_mode( NetSocketStreamReadMode mode );
						NetSocketStreamReadMode const& current_read_mode( ) const;
						NetSocketStreamImpl& set_read_predicate( std::function < std::pair<NetSocketStreamImpl::match_iterator_t, bool>( NetSocketStreamImpl::match_iterator_t begin, NetSocketStreamImpl::match_iterator_t end ) > match_function );
						NetSocketStreamImpl& clear_read_predicate( );
						NetSocketStreamImpl& set_read_until_values( std::string values, bool is_regex );
						daw::nodepp::lib::net::RawSocket socket( );

						std::size_t& buffer_size( );

						NetSocketStreamImpl& set_timeout( int32_t value );

						NetSocketStreamImpl& set_no_delay( bool noDelay );
						NetSocketStreamImpl& set_keep_alive( bool keep_alive, int32_t initial_delay );

						std::string remote_address( ) const;
						std::string local_address( ) const;
						uint16_t remote_port( ) const;
						uint16_t local_port( ) const;

						std::size_t bytes_read( ) const;
						std::size_t bytes_written( ) const;

						//////////////////////////////////////////////////////////////////////////
						/// Callbacks

						//////////////////////////////////////////////////////////////////////////
						/// Summary: Event emitted when a connection is established
						NetSocketStreamImpl& on_connected( std::function<void( )> listener );

						//////////////////////////////////////////////////////////////////////////
						/// Summary: Event emitted when a connection is established
						NetSocketStreamImpl& on_next_connected( std::function<void( )> listener );

						//////////////////////////////////////////////////////////////////////////
						/// StreamReadable

						void emit_connect( );
						void emit_timeout( );

					private:

						static void handle_connect( std::weak_ptr<NetSocketStreamImpl> obj, base::ErrorCode const & err, daw::nodepp::lib::net::Resolver::iterator it );
						static void handle_read( std::weak_ptr<NetSocketStreamImpl> obj, std::shared_ptr<daw::nodepp::base::stream::StreamBuf> read_buffer, base::ErrorCode const & err, std::size_t const & bytes_transfered );
						static void handle_write( std::weak_ptr<daw::thread::Semaphore<int>> outstanding_writes, std::weak_ptr<NetSocketStreamImpl> obj, daw::nodepp::base::write_buffer buff, base::ErrorCode const & err, size_t const & bytes_transfered );

						void write_async( daw::nodepp::base::write_buffer buff );
					};	// struct NetSocketStreamImpl
				}	// namespace impl

				NetSocketStream& operator<<( NetSocketStream &socket, boost::string_ref message );
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw