#pragma once

#include <string>

#include "base_event_emitter.h"
#include "base_types.h"
#include "base_enoding.h"

namespace daw {
	namespace nodepp {
		namespace base {
			namespace stream {
				using namespace daw::nodepp;
				class StreamWritable;
				//////////////////////////////////////////////////////////////////////////
				// Summary:		Readable stream class.
				// Requires:	base::EventEmitter, base::Encoding, data_t, options_t, 
				//				base::stream::StreamWriteable
				class StreamReadable: public virtual base::EventEmitter {
				public:
					virtual std::vector<std::string> const & valid_events( ) const override;
					StreamReadable( ) = default;
					StreamReadable( StreamReadable const & ) = default;
					StreamReadable& operator=( StreamReadable const & ) = default;
					virtual ~StreamReadable( ) = default;
					StreamReadable( StreamReadable && other );
					StreamReadable& operator=(StreamReadable && rhs);

					virtual base::data_t read( ) = 0;
					virtual base::data_t read( size_t bytes ) = 0;

					virtual void set_encoding( base::Encoding const & encoding ) = 0;
					virtual void resume( ) = 0;
					virtual void pause( ) = 0;
					virtual StreamWritable& pipe( StreamWritable& destination ) = 0;
					virtual StreamWritable& pipe( StreamWritable& destination, base::options_t options ) = 0;
					virtual void unpipe( StreamWritable& destination ) = 0;
					virtual void unshift( data_t const & chunk ) = 0;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when data is received
					virtual void when_data_recv( std::function<void( std::shared_ptr<base::data_t>, bool )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when data is received
					virtual void when_next_data_recv( std::function<void( std::shared_ptr<base::data_t>, bool )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when of of stream is read.
					virtual void when_eof( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when of of stream is read.
					virtual void when_next_eof( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the stream is closed
					virtual void when_closed( std::function<void( )> listener );

				protected:
					virtual void emit_data( std::shared_ptr<base::data_t> buffer, bool end_of_file );
					virtual void emit_end( );
					virtual void emit_close( );

				};	// class StreamReadable

				//////////////////////////////////////////////////////////////////////////
				// Summary:		Writable stream class.
				// Requires:	base::EventEmitter, data_t				 
				class StreamWritable: public virtual daw::nodepp::base::EventEmitter {
				public:
					StreamWritable( ) = default;
					StreamWritable( StreamWritable const & ) = default;
					StreamWritable& operator=( StreamWritable const & ) = default;
					virtual ~StreamWritable( ) = default;
					StreamWritable( StreamWritable && other );
					StreamWritable& operator=(StreamWritable && rhs);
					virtual std::vector<std::string> const & valid_events( ) const override;
					virtual void write( base::data_t const & chunk ) = 0;
					virtual void write( std::string const & chunk, base::Encoding const & encoding ) = 0;
					virtual void end( ) = 0;
					virtual void end( base::data_t const & chunk ) = 0;
					virtual void end( std::string const & chunk, base::Encoding const & encoding ) = 0;
					
					template<typename Listener>
					bool write( base::data_t const & chunk, Listener listener ) {
						return base::rollback_event_on_exception( this, "drain", listener, [&]( ) {
							return write( chunk );
						} );
					}

					template<typename Listener>
					bool end( base::data_t const & chunk, Listener listener ) {
						return base::rollback_event_on_exception( this, "finish", listener, [&]( ) {
							return end( chunk );
						} );
					}

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when a write is completed
					virtual void when_a_write_completes( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when end( ... ) has been called and all data
					/// has been flushed
					virtual void when_all_writes_complete( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted whenever this StreamWritable is passed to pipe( )
					///  on a StreamReadable					
					virtual void when_piped( std::function<void( StreamReadable& )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted whenever this StreamWritable is passed to unpipe( )
					///  on a StreamReadable
					virtual void when_unpiped( std::function<void( StreamReadable& )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when end( ... ) has been called and all data
					/// has been flushed
					virtual void when_next_all_writes_complete( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the next write is completed
					virtual void when_next_write_completes( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted whenever this StreamWritable is passed to pipe( )
					///  on a StreamReadable					
					virtual void when_next_pipe( std::function<void( StreamReadable& )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted whenever this StreamWritable is passed to unpipe( )
					///  on a StreamReadable
					virtual void when_next_unpipe( std::function<void( StreamReadable& )> listener );

				protected:
					virtual void emit_drain( );
					virtual void emit_finish( );
					virtual void emit_pipe( );
					virtual void emit_unpipe( );

				};	// class StreamWriteable

				StreamWritable& operator<<(StreamWritable& stream, std::string const & value);
				StreamWritable& operator<<(StreamWritable& stream, base::data_t const & value);

				//////////////////////////////////////////////////////////////////////////
				// Summary:		A duplex stream
				// Requires:	StreamReadable, StreamWritable
				class Stream: public StreamReadable, public StreamWritable {
				public:
					Stream( ) = default;
					Stream( Stream const & ) = default;
					Stream& operator=(Stream const &) = default;
					Stream( Stream && other );
					Stream& operator=(Stream && rhs);
					virtual std::vector<std::string> const & valid_events( ) const override;
					virtual ~Stream( ) = default;

				};

				//////////////////////////////////////////////////////////////////////////
				// Summary:		A duplex stream that transforms the stream on each chunk
				//				going in or coming out
				// Requires:	Stream
				class StreamTransform: public Stream {
				public:
					virtual StreamTransform& transform_read( std::function<void( base::data_t )> listener ) = 0;
					virtual StreamTransform& transform_write( std::function<void( base::data_t )> listener ) = 0;
				};
			}	//namespace stream
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
