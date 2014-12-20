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
				class StreamReadable {
					std::weak_ptr<EventEmitter> m_emitter;
				public:
					StreamReadable( std::weak_ptr<EventEmitter> emitter );
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
					void on_data_recv( std::function<void( std::shared_ptr<base::data_t>, bool )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when data is received
					void on_next_data_recv( std::function<void( std::shared_ptr<base::data_t>, bool )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when of of stream is read.
					void on_eof( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when of of stream is read.
					void on_next_eof( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the stream is closed
					void on_closed( std::function<void( )> listener );

				protected:
					void emit_data( std::shared_ptr<base::data_t> buffer, bool end_of_file );
					void emit_end( );
					void emit_close( );

				};	// class StreamReadable

				//////////////////////////////////////////////////////////////////////////
				// Summary:		Writable stream class.
				// Requires:	base::EventEmitter, data_t				 
				class StreamWritable {
					std::weak_ptr<EventEmitter> m_emitter;
				public:
					StreamWritable( std::weak_ptr<EventEmitter> emitter );
					StreamWritable( StreamWritable const & ) = default;
					StreamWritable& operator=( StreamWritable const & ) = default;
					virtual ~StreamWritable( ) = default;
					StreamWritable( StreamWritable && other );
					StreamWritable& operator=(StreamWritable && rhs);
					
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
					void on_a_write_completes( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when end( ... ) has been called and all data
					/// has been flushed
					void on_all_writes_complete( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted whenever this StreamWritable is passed to pipe( )
					///  on a StreamReadable					
					void on_piped( std::function<void( StreamReadable& )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted whenever this StreamWritable is passed to unpipe( )
					///  on a StreamReadable
					void on_unpiped( std::function<void( StreamReadable& )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when end( ... ) has been called and all data
					/// has been flushed
					void on_next_all_writes_complete( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the next write is completed
					void on_next_write_completes( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted whenever this StreamWritable is passed to pipe( )
					///  on a StreamReadable					
					void on_next_pipe( std::function<void( StreamReadable& )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted whenever this StreamWritable is passed to unpipe( )
					///  on a StreamReadable
					void on_next_unpipe( std::function<void( StreamReadable& )> listener );

				protected:
					void emit_drain( );
					void emit_finish( );
					void emit_pipe( );
					void emit_unpipe( );

				};	// class StreamWriteable

				StreamWritable& operator<<(StreamWritable& stream, std::string const & value);
				StreamWritable& operator<<(StreamWritable& stream, base::data_t const & value);

				//////////////////////////////////////////////////////////////////////////
				// Summary:		A duplex stream
				// Requires:	StreamReadable, StreamWritable
				class Stream: public StreamReadable, public StreamWritable {
					std::weak_ptr<EventEmitter> m_emitter;
				public:
					Stream( std::weak_ptr<EventEmitter> emitter );
					Stream( Stream const & ) = default;
					Stream& operator=(Stream const &) = default;
					Stream( Stream && other );
					Stream& operator=(Stream && rhs);
					virtual ~Stream( ) = default;
				};
				
			}	//namespace stream
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
