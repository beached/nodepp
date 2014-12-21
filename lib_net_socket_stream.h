#pragma once

#include <boost/asio.hpp>
#include <cstdint>
#include <string>

#include "base_enoding.h"
#include "base_error.h"
#include "base_service_handle.h"
#include "base_stream.h"
#include "base_types.h"
#include "lib_net_socket_stream.h"


namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
					using namespace daw::nodepp;
					using namespace boost::asio::ip;

					struct write_buffer;

					struct NetSocketStream: public std::enable_shared_from_this<NetSocketStream>, public base::StandardEvents<NetSocketStream>, public base::stream::StreamReadableEvents<NetSocketStream>, public base::stream::StreamWritableEvents<NetSocketStream> {
						enum class ReadUntil { newline, buffer_full, predicate, next_byte, regex, values };
						using match_iterator_t = boost::asio::buffers_iterator < boost::asio::streambuf::const_buffers_type > ;
						using match_function_t = std::function < std::pair<match_iterator_t, bool>( match_iterator_t begin, match_iterator_t end ) > ;
					private:
						boost::asio::ip::tcp::socket m_socket;
						base::SharedEventEmitter m_emitter;
						
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
							std::shared_ptr<NetSocketStream::match_function_t> read_predicate;
							std::string read_until_values;
							netsockstream_readoptions_t( ) :read_mode( ReadUntil::newline ), max_read_size( 8192 ), read_predicate( ) { }
							netsockstream_readoptions_t( size_t max_read_size_ ) :read_mode( ReadUntil::newline ), max_read_size( max_read_size_ ), read_predicate( ) { }
						} m_read_options;

						std::shared_ptr<std::atomic_int_least32_t> m_outstanding_writes;
						base::data_t m_response_buffers;
						std::size_t m_bytes_read;
						std::size_t m_bytes_written;
					public:
						NetSocketStream( base::SharedEventEmitter emitter = base::create_shared_event_emitter( ) );
						NetSocketStream( boost::asio::io_service& io_service, std::size_t max_read_size, std::shared_ptr<base::EventEmitter> emitter = std::make_shared<base::EventEmitter>( ) );
						NetSocketStream( NetSocketStream&& other );
						NetSocketStream& operator=(NetSocketStream&& rhs);
						~NetSocketStream( );

						NetSocketStream( NetSocketStream const & ) = default;
						NetSocketStream& operator=(NetSocketStream const &) = default;

						std::shared_ptr<NetSocketStream> get_ptr( );

						void read_async( std::shared_ptr<boost::asio::streambuf> read_buffer = nullptr );
						base::data_t read( );
						base::data_t read( std::size_t bytes );

						void write_async( base::data_t const & chunk );
						void write_async( boost::string_ref chunk, base::Encoding const & encoding = base::Encoding( ) );

						void end( );
						void end( base::data_t const & chunk );
						void end( boost::string_ref chunk, base::Encoding const & encoding = base::Encoding( ) );

						void connect( std::string host, uint16_t port );
						void connect( std::string path );

						void close( bool emit_cb = true );
						void cancel( );

						bool is_open( ) const;
						bool is_closed( ) const;
						bool can_write( ) const;

						void set_read_mode( NetSocketStream::ReadUntil mode );
						ReadUntil const& current_read_mode( ) const;
						void set_read_predicate( std::function < std::pair<NetSocketStream::match_iterator_t, bool>( NetSocketStream::match_iterator_t begin, NetSocketStream::match_iterator_t end ) > match_function );
						void clear_read_predicate( );
						void set_read_until_values( std::string values, bool is_regex );
						boost::asio::ip::tcp::socket & socket( );						


						std::size_t& buffer_size( );

						void set_timeout( int32_t value );

						void set_no_delay( bool noDelay );
						void set_keep_alive( bool keep_alive, int32_t initial_delay );

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
						void on_connected( std::function<void( )> listener );

						//////////////////////////////////////////////////////////////////////////
						/// Summary: Event emitted when a connection is established
						void on_next_connected( std::function<void( )> listener );

						//////////////////////////////////////////////////////////////////////////
						/// StreamReadable
						
					private:				

						bool dec_outstanding_writes( );
						void inc_outstanding_writes( );

						static void handle_connect( std::shared_ptr<NetSocketStream> obj, boost::system::error_code const & err, tcp::resolver::iterator it );
						static void handle_read( std::shared_ptr<NetSocketStream> obj, std::shared_ptr<boost::asio::streambuf> read_buffer, boost::system::error_code const & err, std::size_t bytes_transfered );
						static void handle_write( std::shared_ptr<NetSocketStream> obj, write_buffer buff, boost::system::error_code const & err, size_t bytes_transfered );

						void write_async( write_buffer buff );

						virtual void emit_connect( );
						virtual void emit_timeout( );

					};	// struct NetSocketStream

					using SharedNetSocketStream = std::shared_ptr < NetSocketStream > ;

					template<typename... Args>
					SharedNetSocketStream create_shared_socket( Args&&... args ) {
						return std::shared_ptr<NetSocketStream>( new NetSocketStream( std::forward<Args>( args )... ) );
					}

					struct write_buffer {
						using data_type = base::data_t::pointer;
						std::shared_ptr<base::data_t> buff;

						template<typename Iterator>
						write_buffer( Iterator first, Iterator last ) : buff( std::make_shared<base::data_t>( first, last ) ) { }

						write_buffer( base::data_t const & source );
						write_buffer( boost::string_ref source );
						std::size_t size( ) const;

						data_type data( ) const;
						boost::asio::mutable_buffers_1 asio_buff( ) const;
					};	// struct write_buffer
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
