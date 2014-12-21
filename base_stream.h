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
					base::EventEmitter m_emitter;
				public:
					StreamReadableEvents( base::EventEmitter emitter ) : m_emitter( std::move( emitter ) ) { }
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
				template<typename Class>
				struct StreamReadable {
					virtual ~StreamReadable( ) = default;
					virtual base::data_t read( ) = 0;
					virtual base::data_t read( size_t bytes ) = 0;
				};	// class StreamReadable

				template<typename Class>
				class StreamWritableEvents {
					base::EventEmitter m_emitter;
				public:
					StreamWritableEvents( base::EventEmitter emitter ) : m_emitter( std::move(emitter ) ) { }

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
					void emit_write_completion( ) {
						m_emitter->emit( "write_completion" );
					}

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	All async writes have completed
					void emit_all_writes_completed( ) {
						m_emitter->emit( "all_writes_completed" );
					}

				};

				//////////////////////////////////////////////////////////////////////////
				// Summary:		Writable stream class.
				// Requires:	base::EventEmitter, data_t				 
				template<typename Class>
				struct StreamWritable {
					virtual ~StreamWritable( ) = default;
					virtual void write( base::data_t const & chunk ) = 0;
					virtual void write( boost::string_ref chunk, base::Encoding const & encoding ) = 0;
					virtual void end( ) = 0;
					virtual void end( base::data_t const & chunk ) = 0;
					virtual void end( boost::string_ref chunk, base::Encoding const & encoding ) = 0;
					
				};	// class StreamWriteable

				template<typename Class>
				StreamWritable<Class>& operator<<(StreamWritable<Class>& stream, boost::string_ref value) {
					stream.write( value, base::Encoding( ) );
					return stream;
				}


				template<typename Class>
				StreamWritable<Class>& operator<<(StreamWritable<Class>& stream, base::data_t const & value) {
					stream.write( value );
					return stream;
				}

				//////////////////////////////////////////////////////////////////////////
				// Summary:		A duplex stream
				// Requires:	StreamReadable, StreamWritable
				template<typename Class>
				class Stream: public StreamReadable<Class>, public StreamWritable<Class> { 
					virtual ~Stream( ) = default;
				};
				
			}	//namespace stream
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
