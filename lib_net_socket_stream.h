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


namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {		
				namespace impl { class NetSocketStreamImpl; }
				using namespace daw::nodepp;				

				class NetSocketStream: public base::stream::Stream {
					std::shared_ptr<impl::NetSocketStreamImpl> m_impl;
				public:
					NetSocketStream( );
					explicit NetSocketStream( boost::asio::io_service& io_service, size_t max_read_size = 8192 );

					NetSocketStream( NetSocketStream const & rhs);
					NetSocketStream( NetSocketStream&& other );
					NetSocketStream& operator=(NetSocketStream rhs);
					virtual ~NetSocketStream( ) = default;


					enum class ReadUntil { newline, buffer_full, predicate, next_byte, regex, values };
					using match_iterator_t = boost::asio::buffers_iterator < boost::asio::streambuf::const_buffers_type > ;
					using match_function_t = std::function < std::pair<match_iterator_t, bool>( match_iterator_t begin, match_iterator_t end ) > ;
				
					virtual std::vector<std::string> const & valid_events( ) const override;

					NetSocketStream& set_read_mode( ReadUntil mode );
					ReadUntil const& current_read_mode( ) const;
					NetSocketStream& set_read_predicate( std::function < std::pair<match_iterator_t, bool>( match_iterator_t begin, match_iterator_t end ) > match_function );
					NetSocketStream& clear_read_predicate( );
					NetSocketStream& set_read_until_values( std::string const & values, bool is_regex = false );

					boost::asio::ip::tcp::socket & socket( );
					boost::asio::ip::tcp::socket const & socket( ) const;

					NetSocketStream& connect( std::string host, uint16_t port );
					NetSocketStream& connect( std::string path );

					std::size_t& buffer_size( );
					std::size_t const & buffer_size( ) const;					

					NetSocketStream& set_timeout( int32_t value );

					NetSocketStream& set_no_delay( bool noDelay = true );
					NetSocketStream& set_keep_alive( bool keep_alive = false, int32_t initial_delay = 0 );
							
					std::string remote_address( ) const;
					std::string local_address( ) const;
					uint16_t remote_port( ) const;	
					uint16_t local_port( ) const;
					
					std::size_t bytes_read( ) const;
					std::size_t bytes_written( ) const;

					bool is_open( ) const;

					void read_async( );

					// StreamReadable Interface
					virtual base::data_t read( ) override;
					virtual base::data_t read( std::size_t bytes ) override;

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
					virtual NetSocketStream& when_a_write_completes( std::function<void( )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when end( ... ) has been called and all data
					/// has been flushed
					/// Inherited from StreamWritable
					virtual NetSocketStream& when_all_writes_complete( std::function<void( )> listener ) override;					

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
					virtual NetSocketStream& when_next_all_writes_complete( std::function<void( )> listener ) override;	
					
					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the next write is completed
					/// Inherited from StreamWritable
					virtual NetSocketStream& when_next_write_completes( std::function<void( )> listener ) override;
				};	// class NetSocketStream
				
				NetSocketStream& operator<<(NetSocketStream& net_socket, std::string const & value);

				NetSocketStream& operator<<(NetSocketStream& net_socket, base::data_t const & value);


			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
