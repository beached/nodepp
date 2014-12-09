#pragma once
#include <atomic>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/system/error_code.hpp>
#include <cstdint>
#include <memory>
#include <mutex>
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
					struct write_buffer {
						std::shared_ptr<base::data_t> buff;

						template<typename Iterator>
						write_buffer( Iterator first, Iterator last ) : buff( std::make_shared<base::data_t>( first, last ) ) { }

						write_buffer( base::data_t const & source ) : buff( std::make_shared<base::data_t>( source ) ) { }

						write_buffer( std::string const & source ) : buff( std::make_shared<base::data_t>( source.begin( ), source.end( ) ) ) { }

						size_t size( ) const {
							return buff->size( );
						}

						auto data( ) const -> decltype(buff->data( )) {
							return buff->data( );
						}

						boost::asio::mutable_buffers_1 asio_buff( ) const {
							return boost::asio::buffer( data( ), size( ) );
						}
					};
				}


				class NetSocket: public base::stream::Stream {
				public:
					enum class ReadUntil { newline, buffer_full, predicate, next_byte };
					using match_iterator_t = boost::asio::buffers_iterator < boost::asio::streambuf::const_buffers_type > ;
					using match_function_t = std::function < std::pair<match_iterator_t, bool>( match_iterator_t begin, match_iterator_t end ) > ;
				private:
					SocketHandle m_socket;
					std::shared_ptr<boost::asio::streambuf> m_response_buffer;
					std::shared_ptr<base::data_t> m_response_buffers;
					size_t m_bytes_read;
					size_t m_bytes_written;
					std::mutex m_response_buffers_mutex;					
					ReadUntil m_read_mode;
					std::shared_ptr<match_function_t> m_read_predicate;
					std::atomic_int_least32_t m_outstanding_writes;
					bool m_end;
					void inc_outstanding_writes( );
					bool dec_outstanding_writes( );
					void handle_read( boost::system::error_code const & err, size_t bytes_transfered );					
					void handle_write( impl::write_buffer buff, boost::system::error_code const & err );
					void do_async_read( );

					NetSocket& write( impl::write_buffer buff );
				public:
					virtual std::vector<std::string> const & valid_events( ) const override;

					NetSocket& set_read_mode( ReadUntil mode );
					ReadUntil const& current_read_mode( ) const;
					NetSocket& set_read_predicate( std::function < std::pair<match_iterator_t, bool>( match_iterator_t begin, match_iterator_t end ) > match_function );
					NetSocket& clear_read_predicate( );

					NetSocket( );
					explicit NetSocket( boost::asio::io_service& io_service );
					explicit NetSocket( SocketHandle handle );
					NetSocket( NetSocket const & ) = delete;
					NetSocket& operator=(NetSocket const &) = delete;
					NetSocket( NetSocket&& other );
					NetSocket& operator=(NetSocket&& rhs);
					virtual ~NetSocket( );

					boost::asio::ip::tcp::socket & socket( );
					boost::asio::ip::tcp::socket const & socket( ) const;

					NetSocket& connect( std::string host, uint16_t port );
					NetSocket& connect( std::string path );

					size_t& buffer_size( );
					size_t const & buffer_size( ) const;					

					NetSocket& set_timeout( int32_t value );

					template<typename Listener>
					NetSocket& set_timeout( int32_t value, Listener listener ) {
						return base::rollback_event_on_exception( this, "timeout", listener, [&]( ) -> NetSocket& {
							set_timeout( value );
						} );
					}

					NetSocket& set_no_delay( bool noDelay = true );
					NetSocket& set_keep_alive( bool keep_alive = false, int32_t initial_delay = 0 );
		
					NetSocket& unref( );
					NetSocket& ref( );
					
					std::string remote_address( ) const;
					std::string local_address( ) const;
					uint16_t remote_port( ) const;	
					uint16_t local_port( ) const;
					
					size_t bytes_read( ) const;
					size_t bytes_written( ) const;

					bool is_open( ) const;

					// Event callbacks

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when a connection is established
					NetSocket& on_connect( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when an error occurs
					/// Inherited from EventEmitter
					virtual NetSocket& on_error( std::function<void( base::Error )> listener ) override;

					// StreamReadable callbacks

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when data is received
					/// Inherited from StreamReadable
					virtual NetSocket& on_data( std::function<void( std::shared_ptr<base::data_t>, bool )> listener ) override;
					
					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when of of stream is read.
					/// Inherited from StreamReadable
					virtual NetSocket& on_end( std::function<void( )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the stream is closed
					/// Inherited from StreamReadable
					virtual NetSocket& on_close( std::function<void( )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when end( ... ) has been called and all data
					/// has been flushed
					/// Inherited from StreamWritable
					virtual NetSocket& on_finish( std::function<void( )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted whenever this StreamWritable is passed to 
					/// pipe( ) on a StreamReadable
					/// Inherited from StreamWritable (Not implemented yet)
					virtual NetSocket& on_pipe( std::function<void( StreamReadable& )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted whenever this StreamWritable is passed to 
					/// unpipe( ) on a StreamReadable
					/// Inherited from StreamWritable (Not implemented yet)
					virtual NetSocket& on_unpipe( std::function<void( StreamReadable& )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when a connection is established
					NetSocket& once_connect( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when an error occurs
					/// Inherited from EventEmitter
					virtual NetSocket& once_error( std::function<void( base::Error )> listener ) override;

					// StreamReadable callbacks

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when data is received
					/// Inherited from StreamReadable
					virtual NetSocket& once_data( std::function<void( std::shared_ptr<base::data_t>, bool )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when of of stream is read.
					/// Inherited from StreamReadable
					virtual NetSocket& once_end( std::function<void( )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the stream is closed
					/// Inherited from StreamReadable
					virtual NetSocket& once_close( std::function<void( )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when end( ... ) has been called and all data
					/// has been flushed
					/// Inherited from StreamWritable
					virtual NetSocket& once_finish( std::function<void( )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted whenever this StreamWritable is passed to 
					/// pipe( ) on a StreamReadable
					/// Inherited from StreamWritable (Not implemented yet)
					virtual NetSocket& once_pipe( std::function<void( StreamReadable& )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted whenever this StreamWritable is passed to 
					/// unpipe( ) on a StreamReadable
					/// Inherited from StreamWritable (Not implemented yet)
					virtual NetSocket& once_unpipe( std::function<void( StreamReadable& )> listener ) override;



					// StreamReadable Interface
					virtual base::data_t read( ) override;
					virtual base::data_t read( size_t bytes ) override;

					virtual NetSocket& set_encoding( base::Encoding const & encoding ) override;
					virtual NetSocket& resume( ) override;
					virtual NetSocket& pause( ) override;
					virtual StreamWritable& pipe( StreamWritable& destination ) override;
					virtual StreamWritable& pipe( StreamWritable& destination, base::options_t options ) override;

					virtual NetSocket& unpipe( StreamWritable& destination ) override;
					virtual NetSocket& unshift( base::data_t const & chunk ) override;

					// StreamWritable Interface
					virtual NetSocket& write( base::data_t const & chunk ) override;
					virtual NetSocket& write( std::string const & chunk, base::Encoding const & encoding = base::Encoding( ) ) override;

					virtual void end( ) override;
					virtual void end( base::data_t const & chunk ) override;
					virtual void end( std::string const & chunk, base::Encoding const & encoding = base::Encoding( ) ) override;


					void close( );
				};
				
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
