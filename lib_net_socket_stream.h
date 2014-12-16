#pragma once
#include <atomic>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/system/error_code.hpp>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include "base_enoding.h"
#include "base_error.h"
#include "base_event_emitter.h"
#include "base_service_handle.h"
#include "base_stream.h"
#include "base_types.h"
#include "lib_net_address.h"
#include "lib_net_socket_handle.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				using namespace daw::nodepp;
				namespace impl {
					class write_buffer {
						using data_type = base::data_t::pointer;
					public:
						std::shared_ptr<base::data_t> buff;

						template<typename Iterator>
						write_buffer( Iterator first, Iterator last ) : buff( std::make_shared<base::data_t>( first, last ) ) { }
						
						write_buffer( base::data_t const & source );
						write_buffer( std::string const & source );
						size_t size( ) const;
						
						data_type data( ) const;
						boost::asio::mutable_buffers_1 asio_buff( ) const;
					};
				}	// namespace impl


				class NetSocketStream: public base::stream::Stream {
				public:
					enum class ReadUntil { newline, buffer_full, predicate, next_byte, regex, values };
					using match_iterator_t = boost::asio::buffers_iterator < boost::asio::streambuf::const_buffers_type > ;
					using match_function_t = std::function < std::pair<match_iterator_t, bool>( match_iterator_t begin, match_iterator_t end ) > ;
				private:
					SocketHandle m_socket;
					std::shared_ptr<boost::asio::streambuf> m_response_buffer;
					std::shared_ptr<base::data_t> m_response_buffers;
					size_t m_bytes_read;
					size_t m_bytes_written;
					ReadUntil m_read_mode;
					std::shared_ptr<match_function_t> m_read_predicate;
					std::shared_ptr<std::atomic_int_least32_t> m_outstanding_writes;
					bool m_closed;
					bool m_end;
					std::string m_read_until_values;
					void inc_outstanding_writes( );
					bool dec_outstanding_writes( );
					void handle_read( boost::system::error_code const & err, size_t bytes_transfered );					
					void handle_write( impl::write_buffer buff, boost::system::error_code const & err );					

					NetSocketStream& write( impl::write_buffer buff );
				public:
					virtual std::vector<std::string> const & valid_events( ) const override;

					NetSocketStream& set_read_mode( ReadUntil mode );
					ReadUntil const& current_read_mode( ) const;
					NetSocketStream& set_read_predicate( std::function < std::pair<match_iterator_t, bool>( match_iterator_t begin, match_iterator_t end ) > match_function );
					NetSocketStream& clear_read_predicate( );
					NetSocketStream& set_read_until_values( std::string const & values, bool is_regex = false );

					NetSocketStream( );
					explicit NetSocketStream( boost::asio::io_service& io_service, size_t max_read_size = 8192 );

					NetSocketStream( NetSocketStream const & ) = delete;
					NetSocketStream& operator=(NetSocketStream const &) = delete;
					NetSocketStream( NetSocketStream&& other );
					NetSocketStream& operator=(NetSocketStream&& rhs);
					virtual ~NetSocketStream( ) = default;

					boost::asio::ip::tcp::socket & socket( );
					boost::asio::ip::tcp::socket const & socket( ) const;

					NetSocketStream& connect( std::string host, uint16_t port );
					NetSocketStream& connect( std::string path );

					size_t& buffer_size( );
					size_t const & buffer_size( ) const;					

					NetSocketStream& set_timeout( int32_t value );

					template<typename Listener>
					NetSocketStream& set_timeout( int32_t value, Listener listener ) {
						return base::rollback_event_on_exception( this, "timeout", listener, [&]( ) -> NetSocketStream& {
							set_timeout( value );
						} );
					}

					NetSocketStream& set_no_delay( bool noDelay = true );
					NetSocketStream& set_keep_alive( bool keep_alive = false, int32_t initial_delay = 0 );
		
					NetSocketStream& unref( );
					NetSocketStream& ref( );
					
					std::string remote_address( ) const;
					std::string local_address( ) const;
					uint16_t remote_port( ) const;	
					uint16_t local_port( ) const;
					
					size_t bytes_read( ) const;
					size_t bytes_written( ) const;

					bool is_open( ) const;

					void read_async( );

					// StreamReadable Interface
					virtual base::data_t read( ) override;
					virtual base::data_t read( size_t bytes ) override;

					virtual NetSocketStream& set_encoding( base::Encoding const & encoding ) override;
					virtual NetSocketStream& resume( ) override;
					virtual NetSocketStream& pause( ) override;
					virtual StreamWritable& pipe( StreamWritable& destination ) override;
					virtual StreamWritable& pipe( StreamWritable& destination, base::options_t options ) override;

					virtual NetSocketStream& unpipe( StreamWritable& destination ) override;
					virtual NetSocketStream& unshift( base::data_t const & chunk ) override;

					// StreamWritable Interface
					virtual NetSocketStream& write( base::data_t const & chunk ) override;
					virtual NetSocketStream& write( std::string const & chunk, base::Encoding const & encoding = base::Encoding( ) ) override;

					virtual void end( ) override;
					virtual void end( base::data_t const & chunk ) override;
					virtual void end( std::string const & chunk, base::Encoding const & encoding = base::Encoding( ) ) override;

					void close( bool emit_cb = true );
					void cancel( );

					bool is_closed( ) const;
					bool can_write( ) const;
					// Event callbacks

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when a connection is established
					NetSocketStream& when_connected( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when an error occurs
					/// Inherited from EventEmitter
					virtual NetSocketStream& when_error( std::function<void( base::Error )> listener ) override;

					// StreamReadable callbacks

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when data is received
					/// Inherited from StreamReadable
					virtual NetSocketStream& when_data_recv( std::function<void( std::shared_ptr<base::data_t>, bool )> listener ) override;
					
					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when of of stream is read.
					/// Inherited from StreamReadable
					virtual NetSocketStream& when_eof( std::function<void( )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the stream is closed
					/// Inherited from StreamReadable
					virtual NetSocketStream& when_closed( std::function<void( )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when a write is completed
					/// Inherited from StreamWritable
					virtual NetSocketStream& when_write_completes( std::function<void( )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when end( ... ) has been called and all data
					/// has been flushed
					/// Inherited from StreamWritable
					virtual NetSocketStream& when_writes_finished( std::function<void( )> listener ) override;					

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when a connection is established
					NetSocketStream& when_next_connected( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when an error occurs
					/// Inherited from EventEmitter
					virtual NetSocketStream& when_next_error( std::function<void( base::Error )> listener ) override;

					// StreamReadable callbacks

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when data is received
					/// Inherited from StreamReadable
					virtual NetSocketStream& when_next_data_recv( std::function<void( std::shared_ptr<base::data_t>, bool )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when of of stream is read.
					/// Inherited from StreamReadable
					virtual NetSocketStream& when_next_eof( std::function<void( )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the stream is closed
					/// Inherited from StreamReadable
					virtual NetSocketStream& on_closed( std::function<void( )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when end( ... ) has been called and all data
					/// has been flushed
					/// Inherited from StreamWritable
					virtual NetSocketStream& when_next_writes_finish( std::function<void( )> listener ) override;					
				};
				
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
