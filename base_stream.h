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
				class StreamReadable: virtual public base::EventEmitter {
				public:
					virtual std::vector<std::string> const & valid_events( ) const override;
					StreamReadable( ) = default;
					StreamReadable( StreamReadable const & ) = default;
					StreamReadable& operator=( StreamReadable const & ) = default;
					virtual ~StreamReadable( ) = default;
					virtual base::data_t read( ) = 0;
					virtual base::data_t read( size_t bytes ) = 0;

					virtual StreamReadable& set_encoding( base::Encoding const & encoding ) = 0;
					virtual StreamReadable& resume( ) = 0;
					virtual StreamReadable& pause( ) = 0;
					virtual StreamWritable& pipe( StreamWritable& destination ) = 0;
					virtual StreamWritable& pipe( StreamWritable& destination, base::options_t options ) = 0;
					virtual StreamReadable& unpipe( StreamWritable& destination ) = 0;
					virtual StreamReadable& unshift( data_t const & chunk ) = 0;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when data is received
					virtual StreamReadable& on_data( std::function<void( std::shared_ptr<base::data_t>, bool )> listener ) = 0;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when of of stream is read.
					virtual StreamReadable& on_end( std::function<void( )> listener ) = 0;
					
					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the stream is closed
					virtual StreamReadable& on_close( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when data is received
					virtual StreamReadable& once_data( std::function<void( std::shared_ptr<base::data_t>, bool )> listener ) = 0;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when of of stream is read.
					virtual StreamReadable& once_end( std::function<void( )> listener ) = 0;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the stream is closed
					virtual StreamReadable& once_close( std::function<void( )> listener );
				};	// class StreamReadable

				//////////////////////////////////////////////////////////////////////////
				// Summary:		Writable stream class.
				// Requires:	base::EventEmitter, data_t				 
				class StreamWritable: virtual public daw::nodepp::base::EventEmitter {
				public:
					StreamWritable( ) = default;
					StreamWritable( StreamWritable const & ) = default;
					StreamWritable& operator=( StreamWritable const & ) = default;
					virtual ~StreamWritable( ) = default;

					virtual std::vector<std::string> const & valid_events( ) const override;
					virtual StreamWritable& write( base::data_t const & chunk ) = 0;
					virtual StreamWritable& write( std::string const & chunk, base::Encoding const & encoding ) = 0;
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
					/// Summary: Event emitted when end( ... ) has been called and all data
					/// has been flushed
					virtual StreamWritable& on_finish( std::function<void( )> listener ) = 0;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted whenever this StreamWritable is passed to pipe( )
					///  on a StreamReadable					
					virtual StreamWritable& on_pipe( std::function<void( StreamReadable& )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted whenever this StreamWritable is passed to unpipe( )
					///  on a StreamReadable
					virtual StreamWritable& on_unpipe( std::function<void( StreamReadable& )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when end( ... ) has been called and all data
					/// has been flushed
					virtual StreamWritable& once_finish( std::function<void( )> listener ) = 0;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted whenever this StreamWritable is passed to pipe( )
					///  on a StreamReadable					
					virtual StreamWritable& once_pipe( std::function<void( StreamReadable& )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted whenever this StreamWritable is passed to unpipe( )
					///  on a StreamReadable
					virtual StreamWritable& once_unpipe( std::function<void( StreamReadable& )> listener );


				};	// class StreamWriteable

				StreamWritable& operator<<(StreamWritable& stream, std::string const & value);
				StreamWritable& operator<<(StreamWritable& stream, base::data_t const & value);

				//////////////////////////////////////////////////////////////////////////
				// Summary:		A duplex stream
				// Requires:	StreamReadable, StreamWritable
				class Stream: public StreamReadable, public StreamWritable {
				public:
					virtual std::vector<std::string> const & valid_events( ) const override;
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
