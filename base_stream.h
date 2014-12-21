#pragma once
#include <boost/utility/string_ref.hpp>
#include <string>

#include "base_event_emitter.h"
#include "base_types.h"
#include "base_enoding.h"

namespace daw {
	namespace nodepp {
		namespace base {
			namespace stream {
				using namespace daw::nodepp;

				template<typename Class>
				class StreamReadableEvents {
					base::SharedEventEmitter m_emitter;
				public:
					StreamReadableEvents( std::shared_ptr<base::EventEmitter> emitter ) : m_emitter( std::move( emitter ) ) { }
					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when data is received
					void on_data_recv( std::function<void( std::shared_ptr<base::data_t>, bool )> listener ) {
						m_emitter->add_listener( "data_recv", listener );
					}

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when data is received
					void on_next_data_recv( std::function<void( std::shared_ptr<base::data_t>, bool )> listener ) {
						m_emitter->add_listener( "data_recv", listener, true );
					}

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when of of stream is read.
					void on_eof( std::function<void( )> listener ) {
						m_emitter->add_listener( "eof", listener );
					}

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when of of stream is read.
					void on_next_eof( std::function<void( )> listener ) {
						m_emitter->add_listener( "eof", listener, true );
					}

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when the stream is closed
					void on_closed( std::function<void( )> listener ) {
						m_emitter->add_listener( "closed", listener );
					}
				protected:
					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Emit an event with the data received and whether the eof
					///				has been reached
					void emit_data_recv( std::shared_ptr<base::data_t> buffer, bool end_of_file ) {
						m_emitter->emit( "data_recv", std::move( buffer ), end_of_file );
					}

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the eof has been reached
					void emit_eof( ) {
						m_emitter->emit( "eof" );
					}

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the socket is closed
					void emit_closed( ) {
						m_emitter->emit( "closed" );
					}
				};

				//////////////////////////////////////////////////////////////////////////
				// Summary:		Readable stream class.
				// Requires:	base::EventEmitter, base::Encoding, data_t, options_t, 
				//				base::stream::StreamWriteable
				class StreamReadable: public StreamReadableEvents<StreamReadable> {
				public:
					StreamReadable( base::SharedEventEmitter emitter );
					StreamReadable( StreamReadable const & ) = default;
					StreamReadable& operator=( StreamReadable const & ) = default;
					virtual ~StreamReadable( ) = default;
					StreamReadable( StreamReadable && other );
					StreamReadable& operator=(StreamReadable && rhs);

					virtual base::data_t read( ) = 0;
					virtual base::data_t read( size_t bytes ) = 0;

				};	// class StreamReadable

				template<typename Class>
				class StreamWritableEvents {
					base::SharedEventEmitter m_emitter;
				public:
					StreamWritableEvents( base::SharedEventEmitter emitter ) : m_emitter( std::move(emitter ) ) { }

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when a pending write is completed
					void on_write_completion( std::function<void( )> listener ) {
						m_emitter->add_listener( "write_completion", listener );
					}

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when the next pending write is completed
					void on_next_write_completion( std::function<void( )> listener ) {
						m_emitter->add_listener( "write_completion", listener );
					}

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when end( ... ) has been called and all
					///				data has been flushed
					void on_all_writes_completed( std::function<void( )> listener ) {
						m_emitter->add_listener( "all_writes_completed", listener );
					}
				
				protected:
					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when an async write completes
					void emit_write_completion( );

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	All async writes have completed
					void emit_all_writes_completed( );

				};

				//////////////////////////////////////////////////////////////////////////
				// Summary:		Writable stream class.
				// Requires:	base::EventEmitter, data_t				 
				struct StreamWritable: public StreamWritableEvents<StreamWritable> {
					StreamWritable( std::weak_ptr<EventEmitter> emitter );
					StreamWritable( StreamWritable const & ) = default;
					StreamWritable& operator=( StreamWritable const & ) = default;
					virtual ~StreamWritable( ) = default;
					StreamWritable( StreamWritable && other );
					StreamWritable& operator=(StreamWritable && rhs);
					
					virtual void write( base::data_t const & chunk ) = 0;
					virtual void write( boost::string_ref chunk, base::Encoding const & encoding ) = 0;
					virtual void end( ) = 0;
					virtual void end( base::data_t const & chunk ) = 0;
					virtual void end( boost::string_ref chunk, base::Encoding const & encoding ) = 0;
					
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

				};	// class StreamWriteable

				StreamWritable& operator<<(StreamWritable& stream, boost::string_ref value);
				StreamWritable& operator<<(StreamWritable& stream, base::data_t const & value);

				//////////////////////////////////////////////////////////////////////////
				// Summary:		A duplex stream
				// Requires:	StreamReadable, StreamWritable
				class Stream: public StreamReadable, public StreamWritable {
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
