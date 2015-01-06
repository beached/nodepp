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

				template<typename Child>
				class StreamWritableEvents {
					Child& child( ) {
						return *static_cast<Child*>(this);
					}

					EventEmitter& emitter( ) {
						return child( ).emitter( );
					}
				public:

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when a pending write is completed
					Child& on_write_completion( std::function<void( )> listener ) {
						emitter( )->add_listener( "write_completion", listener );
						return child( );
					}

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when the next pending write is completed
					Child& on_next_write_completion( std::function<void( )> listener ) {
						emitter( )->add_listener( "write_completion", listener );
						return child( );
					}

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when end( ... ) has been called and all
					///				data has been flushed
					Child& on_all_writes_completed( std::function<void( )> listener ) {
						emitter( )->add_listener( "all_writes_completed", listener );
						return child( );
					}

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when an async write completes
					void emit_write_completion( ) {
						emitter( )->emit( "write_completion" );
					}

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	All async writes have completed
					void emit_all_writes_completed( ) {
						emitter( )->emit( "all_writes_completed" );
					}

				};	// class StreamWritableEvents


				template<typename Child>
				class StreamReadableEvents {
					Child& child( ) {
						return *static_cast<Child*>(this);
					}

					EventEmitter& emitter( ) {
						return child( ).emitter( );
					}
				public:
					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when data is received
					Child& on_data_received( std::function<void( std::shared_ptr<base::data_t>, bool )> listener ) {
						emitter( )->add_listener( "data_received", listener );
						return child( );
					}

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when data is received
					Child& on_next_data_received( std::function<void( std::shared_ptr<base::data_t>, bool )> listener ) {
						emitter( )->add_listener( "data_received", listener, true );
						return child( );
					}

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when of of stream is read.
					Child& on_eof( std::function<void( )> listener ) {
						emitter( )->add_listener( "eof", listener );
						return child( );
					}

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when of of stream is read.
					Child& on_next_eof( std::function<void( )> listener ) {
						emitter( )->add_listener( "eof", listener, true );
						return child( );
					}

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Event emitted when the stream is closed
					Child& on_closed( std::function<void( )> listener ) {
						emitter( )->add_listener( "closed", listener );
						return child( );
					}

					//////////////////////////////////////////////////////////////////////////
					/// Summary:	Emit an event with the data received and whether the eof
					///				has been reached
					void emit_data_received( std::shared_ptr<base::data_t> buffer, bool end_of_file ) {
						emitter( )->emit( "data_received", std::move( buffer ), end_of_file );
					}

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the eof has been reached
					void emit_eof( ) {
						emitter( )->emit( "eof" );
					}

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the socket is closed
					void emit_closed( ) {
						emitter( )->emit( "closed" );
					}

					template<typename StreamWritableObj>
					Child& delegate_data_received_to( std::weak_ptr<StreamWritableObj> stream_writable_obj ) {
						on_data_received( [stream_writable_obj]( base::data_t buff, bool eof ) {
							if( !stream_writable_obj.expired( ) ) {
								stream_writable_obj.lock( )->write( buff );
							}
						} );
					}

				};	// class StreamReadableEvents

				
			}	//namespace stream
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
