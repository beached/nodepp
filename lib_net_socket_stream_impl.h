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
				namespace impl {
					using namespace daw::nodepp;
					using namespace boost::asio::ip;

					using namespace daw::nodepp::lib::net;

					class write_buffer {
						using data_type = base::data_t::pointer;
					public:
						std::shared_ptr<base::data_t> buff;

						template<typename Iterator>
						write_buffer( Iterator first, Iterator last ) : buff( std::make_shared<base::data_t>( first, last ) ) { }

						write_buffer( base::data_t const & source );
						write_buffer( std::string const & source );
						std::size_t size( ) const;

						data_type data( ) const;
						boost::asio::mutable_buffers_1 asio_buff( ) const;
					};

					class NetSocketStreamImpl: public base::stream::Stream, public std::enable_shared_from_this<NetSocketStreamImpl> {
					private:
						boost::asio::ip::tcp::socket m_socket;
						size_t m_max_read_size;
						base::data_t m_response_buffers;
						std::size_t m_bytes_read;
						std::size_t m_bytes_written;
						NetSocketStream::ReadUntil m_read_mode;
						std::unique_ptr<NetSocketStream::match_function_t> m_read_predicate;
						std::atomic_int_least32_t m_outstanding_writes;
						bool m_closed;
						bool m_end;
						std::string m_read_until_values;


						void inc_outstanding_writes( );
						bool dec_outstanding_writes( );
						static void handle_read( std::shared_ptr<NetSocketStreamImpl> self, std::shared_ptr<boost::asio::streambuf> read_buffer, boost::system::error_code const & err, std::size_t bytes_transfered );
						static void handle_write( std::shared_ptr<NetSocketStreamImpl> self, write_buffer buff, boost::system::error_code const & err, size_t bytes_transfered );
						static void handle_connect( std::shared_ptr<NetSocketStreamImpl> self, boost::system::error_code const & err, tcp::resolver::iterator it );
						void write( write_buffer buff );
						friend NetSocketStreamImpl& operator<<(NetSocketStreamImpl&, std::string const &);
						friend NetSocketStreamImpl& operator<<(NetSocketStreamImpl&, base::data_t const &);

					public:
						std::shared_ptr<NetSocketStreamImpl> get_ptr( );
						NetSocketStreamImpl( );
						explicit NetSocketStreamImpl( boost::asio::io_service& io_service, std::size_t max_read_size );

						NetSocketStreamImpl( NetSocketStreamImpl const & ) = delete;
						NetSocketStreamImpl& operator=(NetSocketStreamImpl const &) = delete;
						NetSocketStreamImpl( NetSocketStreamImpl&& other ) = delete;
						NetSocketStreamImpl& operator=(NetSocketStreamImpl&& rhs) = delete;
						virtual ~NetSocketStreamImpl( );

						virtual std::vector<std::string> const & valid_events( ) const override;

						void set_read_mode( NetSocketStream::ReadUntil mode );
						NetSocketStream::ReadUntil const& current_read_mode( ) const;
						void set_read_predicate( std::function < std::pair<NetSocketStream::match_iterator_t, bool>( NetSocketStream::match_iterator_t begin, NetSocketStream::match_iterator_t end ) > match_function );
						void clear_read_predicate( );
						void set_read_until_values( std::string const & values, bool is_regex );


						boost::asio::ip::tcp::socket & socket( );
						boost::asio::ip::tcp::socket const & socket( ) const;

						void connect( std::string host, uint16_t port );
						void connect( std::string path );

						std::size_t& buffer_size( );
						std::size_t const & buffer_size( ) const;

						void set_timeout( int32_t value );

						void set_no_delay( bool noDelay );
						void set_keep_alive( bool keep_alive, int32_t initial_delay );

						std::string remote_address( ) const;
						std::string local_address( ) const;
						uint16_t remote_port( ) const;
						uint16_t local_port( ) const;

						std::size_t bytes_read( ) const;
						std::size_t bytes_written( ) const;

						bool is_open( ) const;

						void read_async( std::shared_ptr<boost::asio::streambuf> read_buffer = nullptr );

						// StreamReadable Interface
						virtual base::data_t read( ) override;
						virtual base::data_t read( std::size_t bytes ) override;

						virtual void set_encoding( base::Encoding const & encoding ) override;
						virtual void resume( ) override;
						virtual void pause( ) override;
						virtual StreamWritable& pipe( StreamWritable& destination ) override;
						virtual StreamWritable& pipe( StreamWritable& destination, base::options_t options ) override;

						virtual void unpipe( StreamWritable& destination ) override;
						virtual void unshift( base::data_t const & chunk ) override;

						// StreamWritable Interface
						virtual void write( base::data_t const & chunk ) override;
						virtual void write( std::string const & chunk, base::Encoding const & encoding ) override;

						virtual void end( ) override;
						virtual void end( base::data_t const & chunk ) override;
						virtual void end( std::string const & chunk, base::Encoding const & encoding ) override;

						void close( bool emit_cb );
						void cancel( );

						bool is_closed( ) const;
						bool can_write( ) const;
						// Event callbacks

						//////////////////////////////////////////////////////////////////////////
						/// Summary: Event emitted when a connection is established
						virtual void on_connected( std::function<void( )> listener );

						//////////////////////////////////////////////////////////////////////////
						/// Summary: Event emitted when a connection is established
						virtual void on_next_connected( std::function<void( )> listener );


					protected:
						virtual void emit_connect( );
						virtual void emit_timeout( );

					};
				}	// namespace impl
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
