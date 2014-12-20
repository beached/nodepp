#include <string>

#include "base_event_emitter.h"
#include "base_stream.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace base {
			namespace stream {
				using namespace daw::nodepp;

				void StreamReadable::emit_data( std::shared_ptr<base::data_t> buffer, bool end_of_file ) {
					m_emitter->emit( "data", std::move( buffer ), end_of_file );
					if( end_of_file ) {
						emit_end( );
					}
				}

				void StreamReadable::emit_end( ) {
					m_emitter->emit( "end" );
				}

				void StreamReadable::emit_close( ) {
					m_emitter->emit( "close" );
				}

				void StreamReadable::on_data_recv( std::function<void( std::shared_ptr<base::data_t>, bool )> listener ) {
					m_emitter->add_listener( "data", listener );
				}

				void StreamReadable::on_next_data_recv( std::function<void( std::shared_ptr<base::data_t>, bool )> listener ) {
					m_emitter->add_listener( "data", listener, true );
				}

				void StreamReadable::on_eof( std::function<void( )> listener ) {
					m_emitter->add_listener( "end", listener );
				}

				void StreamReadable::on_next_eof( std::function<void( )> listener ) {
					m_emitter->add_listener( "end", listener );
				}

				void StreamReadable::on_closed( std::function<void( )> listener ) {
					m_emitter->add_listener( "close", listener );
				}

				void StreamWritable::emit_drain( ) {
					m_emitter->emit( "drain" );
				}

				void StreamWritable::emit_finish( ) {
					m_emitter->emit( "finish" );
				}

				void StreamWritable::emit_pipe( ) {
					m_emitter->emit( "pipe" );
				}

				void StreamWritable::emit_unpipe( ) {
					m_emitter->emit( "unpipe" );
				}				

				void StreamWritable::on_a_write_completes( std::function<void( )> listener ) {
					m_emitter->add_listener( "drain", listener );
				}

				void StreamWritable::on_all_writes_complete( std::function<void( )> listener ) { 
					m_emitter->add_listener( "finish", listener );
				}

				void StreamWritable::on_piped( std::function<void( StreamReadable& )> listener ) { 
					m_emitter->add_listener( "pipe", listener );
				}

				void StreamWritable::on_unpiped( std::function<void( StreamReadable& )> listener ) { 
					m_emitter->add_listener( "unpipe", listener );
				}

				void StreamWritable::on_next_write_completes( std::function<void( )> listener ) { 
					m_emitter->add_listener( "drain", listener, true );
				}

				void StreamWritable::on_next_all_writes_complete( std::function<void( )> listener ) { 
					m_emitter->add_listener( "finish", listener, true );
				}

				void StreamWritable::on_next_pipe( std::function<void( StreamReadable& )> listener ) { 
					m_emitter->add_listener( "pipe", listener, true );
				}

				void StreamWritable::on_next_unpipe( std::function<void( StreamReadable& )> listener ) { 
					m_emitter->add_listener( "unpipe", listener, true );
				}

				StreamWritable& operator<<(StreamWritable& stream, std::string const & value) {
					stream.write( value, base::Encoding( ) );
					return stream;
				}

				StreamWritable& operator<<(StreamWritable& stream, base::data_t const & value) {
					stream.write( value );
					return stream;
				}

				Stream::Stream( std::weak_ptr<EventEmitter> emitter ) : m_emitter( emitter ) { }

				Stream::Stream( Stream && other ) : StreamReadable( std::move( other ) ), StreamWritable( std::move( other ) ), m_emitter( std::move( other.m_emitter ) ) { }

				Stream& Stream::operator=(Stream && rhs) {
					if( this != &rhs ) {
						m_emitter = std::move( rhs.m_emitter );
					}
					return *this;
				}

				StreamWritable::StreamWritable( std::weak_ptr<EventEmitter> emitter ) : m_emitter( emitter ) { }

				StreamWritable::StreamWritable( StreamWritable && other ) : m_emitter( std::move( other.m_emitter ) ) { }

				StreamWritable& StreamWritable::operator=(StreamWritable && rhs) {
					if( this != &rhs ) {
						m_emitter = std::move( rhs.m_emitter );
					}
					return *this;
				}

				StreamReadable::StreamReadable( std::weak_ptr<EventEmitter> emitter ) : m_emitter( emitter ) { }

				StreamReadable::StreamReadable( StreamReadable && other ) :m_emitter( std::move( other.m_emitter ) ) { }

				StreamReadable& StreamReadable::operator=(StreamReadable && rhs) {
					if( this != &rhs ) {
						m_emitter = std::move( rhs.m_emitter );
					}
					return *this;
				}


			}	// namespace stream
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
