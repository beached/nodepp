#pragma once

#include <boost/asio.hpp>
#include <cstdint>
#include <memory>
#include <string>

#include "base_enoding.h"
#include "base_error.h"
#include "base_service_handle.h"
#include "base_stream.h"
#include "base_types.h"
#include "base_write_buffer.h"
#include "lib_net_socket_stream.h"
#include "semaphore.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
					using namespace daw::nodepp;
					using namespace boost::asio::ip;
					namespace impl {
						struct NetSocketStreamImpl;
					}

					using NetSocketStream = std::shared_ptr < impl::NetSocketStreamImpl > ;

					NetSocketStream create_net_socket_stream( base::EventEmitter emitter = base::create_event_emitter( ) );
					NetSocketStream create_net_socket_stream( boost::asio::io_service& io_service, std::size_t max_read_size, base::EventEmitter emitter = base::create_event_emitter( ) );

					namespace impl {
						struct NetSocketStreamImpl: public base::enable_shared<NetSocketStreamImpl>, public base::StandardEvents<NetSocketStreamImpl>, public base::stream::StreamReadableEvents<NetSocketStreamImpl>, public base::stream::StreamWritableEvents<NetSocketStreamImpl> {
							enum class ReadUntil { newline, buffer_full, predicate, next_byte, regex, values, double_newline };
							using match_iterator_t = boost::asio::buffers_iterator < boost::asio::streambuf::const_buffers_type > ;
							using match_function_t = std::function < std::pair<match_iterator_t, bool>( match_iterator_t begin, match_iterator_t end ) > ;
						private:
							std::shared_ptr<boost::asio::ip::tcp::socket> m_socket;
							base::EventEmitter m_emitter;
							
							struct netsockstream_state_t {
								bool closed;
								bool end;
								netsockstream_state_t( ) : closed( false ), end( false ) { }
								netsockstream_state_t( netsockstream_state_t const & ) = default;
								netsockstream_state_t& operator=(netsockstream_state_t const &) = default;
								~netsockstream_state_t( ) = default;
							} m_state;
	
							struct netsockstream_readoptions_t {
								ReadUntil read_mode;
								size_t max_read_size;
								std::unique_ptr<NetSocketStreamImpl::match_function_t> read_predicate;
								std::string read_until_values;
								
								netsockstream_readoptions_t( ) :
									read_mode( ReadUntil::newline ), 
									max_read_size( 8192 ), 
									read_predicate( ) { }
								
								netsockstream_readoptions_t( size_t max_read_size_ ) :
									read_mode( ReadUntil::newline ), 
									max_read_size( max_read_size_ ), 
									read_predicate( ) { }

								netsockstream_readoptions_t( netsockstream_readoptions_t const & ) = delete;
								netsockstream_readoptions_t& operator=(netsockstream_readoptions_t const &) = delete;
								
								inline netsockstream_readoptions_t( netsockstream_readoptions_t && other ):
									read_mode( std::move( other.read_mode ) ),
									max_read_size( std::move( other.max_read_size ) ),
									read_predicate( std::move( other.read_predicate ) ) { }

								inline netsockstream_readoptions_t& operator=(netsockstream_readoptions_t && rhs) {
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
							base::data_t m_response_buffers;
							std::size_t m_bytes_read;
							std::size_t m_bytes_written;

							NetSocketStreamImpl( base::EventEmitter emitter );
							NetSocketStreamImpl( boost::asio::io_service& io_service, std::size_t max_read_size, base::EventEmitter emitter );

						public:
							friend NetSocketStream lib::net::create_net_socket_stream( base::EventEmitter emitter );
							friend NetSocketStream lib::net::create_net_socket_stream( boost::asio::io_service& io_service, std::size_t max_read_size, base::EventEmitter emitter );

							NetSocketStreamImpl( NetSocketStreamImpl&& other );
							NetSocketStreamImpl& operator=(NetSocketStreamImpl&& rhs);
							~NetSocketStreamImpl( );
	
							NetSocketStreamImpl( NetSocketStreamImpl const & ) = delete;
							NetSocketStreamImpl& operator=(NetSocketStreamImpl const &) = delete;
	
							base::EventEmitter& emitter( );

							NetSocketStreamImpl&  read_async( std::shared_ptr<boost::asio::streambuf> read_buffer = nullptr );
							base::data_t read( );
							base::data_t read( std::size_t bytes );
	
							NetSocketStreamImpl& write_async( base::data_t const & chunk );
							NetSocketStreamImpl& write_async( boost::string_ref chunk, base::Encoding const & encoding = base::Encoding( ) );
	
							NetSocketStreamImpl& end( );
							NetSocketStreamImpl& end( base::data_t const & chunk );
							NetSocketStreamImpl& end( boost::string_ref chunk, base::Encoding const & encoding = base::Encoding( ) );
	
							NetSocketStreamImpl& connect( boost::string_ref host, uint16_t port );
	
							void close( bool emit_cb = true );
							void cancel( );
	
							bool is_open( ) const;
							bool is_closed( ) const;
							bool can_write( ) const;
								
							NetSocketStreamImpl& set_read_mode( NetSocketStreamImpl::ReadUntil mode );
							ReadUntil const& current_read_mode( ) const;
							NetSocketStreamImpl& set_read_predicate( std::function < std::pair<NetSocketStreamImpl::match_iterator_t, bool>( NetSocketStreamImpl::match_iterator_t begin, NetSocketStreamImpl::match_iterator_t end ) > match_function );
							NetSocketStreamImpl& clear_read_predicate( );
							NetSocketStreamImpl& set_read_until_values( std::string values, bool is_regex );
							boost::asio::ip::tcp::socket & socket( );
	
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

							static void handle_connect( std::weak_ptr<NetSocketStreamImpl> obj, boost::system::error_code const & err, tcp::resolver::iterator it );
							static void handle_read( std::weak_ptr<NetSocketStreamImpl> obj, std::shared_ptr<boost::asio::streambuf> read_buffer, boost::system::error_code const & err, std::size_t const & bytes_transfered );
							static void handle_write( std::weak_ptr<daw::thread::Semaphore<int>> outstanding_writes, std::weak_ptr<NetSocketStreamImpl> obj, base::write_buffer buff, boost::system::error_code const & err, size_t const & bytes_transfered );
	
							void write_async( base::write_buffer buff );
	
						};	// struct NetSocketStreamImpl

						

					}	// namespace impl
					
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
