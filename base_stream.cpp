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

				StreamReadable& StreamReadable::when_closed( std::function<void( )> listener ) { 
					throw std::runtime_error( "Method not implemented in child" ); 
				}
				
				StreamReadable& StreamReadable::when_next_close( std::function<void( )> listener ) { 
					throw std::runtime_error( "Method not implemented in child" );
				}

				std::vector<std::string> const & StreamWritable::valid_events( ) const {
					static auto const result = [&]( ) {
						auto local = std::vector < std::string > { "drain", "finish", "pipe", "unpipe" };
						return base::impl::append_vector( local, EventEmitter::valid_events( ) );
					}();
					return result;
				}

				std::vector<std::string> const & Stream::valid_events( ) const {
					static auto const result = [&]( ) {
						auto local = StreamReadable::valid_events( );
						return base::impl::append_vector( local, StreamWritable::valid_events( ) );
					}();
					return result;
				}

				StreamWritable& StreamWritable::when_piped( std::function<void( StreamReadable& )> listener ) {
					throw std::runtime_error( "Method not implemented in child" );
				}

				StreamWritable& StreamWritable::when_unpiped( std::function<void( StreamReadable& )> listener ) {
					throw std::runtime_error( "Method not implemented in child" );
				}

				StreamWritable& StreamWritable::when_next_pipe( std::function<void( StreamReadable& )> listener ) {
					throw std::runtime_error( "Method not implemented in child" );
				}

				StreamWritable& StreamWritable::when_next_unpipe( std::function<void( StreamReadable& )> listener ) {
					throw std::runtime_error( "Method not implemented in child" );
				}

				StreamWritable& operator<<(StreamWritable& stream, std::string const & value) {
					return stream.write( value, base::Encoding( ) );
				}

				StreamWritable& operator<<(StreamWritable& stream, base::data_t const & value) {
					return stream.write( value );
				}

			}	// namespace stream
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
