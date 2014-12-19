#include <string>

#include "base_event_emitter.h"
#include "base_stream.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace base {
			namespace stream {
				using namespace daw::nodepp;
				std::vector<std::string> const & StreamReadable::valid_events( ) const {
					static auto const result = [&]( ) {
						auto local = std::vector < std::string > { "data", "end", "close" };
						return base::impl::append_vector( local, EventEmitter::valid_events( ) );
					}();
					return result;
				}

				void StreamReadable::emit_data( std::shared_ptr<base::data_t> buffer, bool end_of_file ) {
					emit( "data", std::move( buffer ), end_of_file );
					if( end_of_file ) {
						emit_end( );
					}
				}

				void StreamReadable::emit_end( ) {
					emit( "end" );
				}

				void StreamReadable::emit_close( ) {
					emit( "close" );
				}

				void StreamReadable::when_data_recv( std::function<void( std::shared_ptr<base::data_t>, bool )> listener ) {
					add_listener( "data", listener );
				}

				void StreamReadable::when_next_data_recv( std::function<void( std::shared_ptr<base::data_t>, bool )> listener ) {
					add_listener( "data", listener, true );
				}

				void StreamReadable::when_eof( std::function<void( )> listener ) {
					add_listener( "end", listener );
				}

				void StreamReadable::when_next_eof( std::function<void( )> listener ) {
					add_listener( "end", listener );
				}

				void StreamReadable::when_closed( std::function<void( )> listener ) {
					add_listener( "close", listener );
				}

				std::vector<std::string> const & StreamWritable::valid_events( ) const {
					static auto const result = [&]( ) {
						auto local = std::vector < std::string > { "drain", "finish", "pipe", "unpipe" };
						return base::impl::append_vector( local, EventEmitter::valid_events( ) );
					}();
					return result;
				}

				void StreamWritable::emit_drain( ) {
					emit( "drain" );
				}

				void StreamWritable::emit_finish( ) {
					emit( "finish" );
				}

				void StreamWritable::emit_pipe( ) {
					emit( "pipe" );
				}

				void StreamWritable::emit_unpipe( ) {
					emit( "unpipe" );
				}				

				void StreamWritable::when_a_write_completes( std::function<void( )> listener ) {
					add_listener( "drain", listener );
				}

				void StreamWritable::when_all_writes_complete( std::function<void( )> listener ) { 
					add_listener( "finish", listener );
				}

				void StreamWritable::when_piped( std::function<void( StreamReadable& )> listener ) { 
					add_listener( "pipe", listener );
				}

				void StreamWritable::when_unpiped( std::function<void( StreamReadable& )> listener ) { 
					add_listener( "unpipe", listener );
				}

				void StreamWritable::when_next_write_completes( std::function<void( )> listener ) { 
					add_listener( "drain", listener, true );
				}

				void StreamWritable::when_next_all_writes_complete( std::function<void( )> listener ) { 
					add_listener( "finish", listener, true );
				}

				void StreamWritable::when_next_pipe( std::function<void( StreamReadable& )> listener ) { 
					add_listener( "pipe", listener, true );
				}

				void StreamWritable::when_next_unpipe( std::function<void( StreamReadable& )> listener ) { 
					add_listener( "unpipe", listener, true );
				}

				StreamWritable& operator<<(StreamWritable& stream, std::string const & value) {
					stream.write( value, base::Encoding( ) );
					return stream;
				}

				StreamWritable& operator<<(StreamWritable& stream, base::data_t const & value) {
					stream.write( value );
					return stream;
				}

				std::vector<std::string> const & Stream::valid_events( ) const {
					static auto const result = [&]( ) {
						auto local = StreamReadable::valid_events( );
						return base::impl::append_vector( local, StreamWritable::valid_events( ) );
					}();
					return result;
				}

// 				Stream::Stream( ) : base::EventEmitter( ), StreamReadable( ), StreamWritable( ) { }
// 				Stream::Stream( Stream const & );
// 				Stream& Stream::operator=(Stream const &);

				Stream::Stream( Stream && other ) : base::EventEmitter( std::move( other ) ), StreamReadable( std::move( other ) ), StreamWritable( std::move( other ) ) { }

				Stream& Stream::operator=(Stream && rhs) {
					if( this != &rhs ) {

					}
					return *this;
				}

				StreamWritable::StreamWritable( StreamWritable && other ) : base::EventEmitter( std::move( other ) ) { }

				StreamWritable& StreamWritable::operator=(StreamWritable && rhs) {
					if( this != &rhs ) {

					}
					return *this;
				}

				StreamReadable::StreamReadable( StreamReadable && other ) : base::EventEmitter( std::move( other ) ) { }

				StreamReadable& StreamReadable::operator=(StreamReadable && rhs) {
					if( this != &rhs ) {

					}
					return *this;
				}


			}	// namespace stream
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
