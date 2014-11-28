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
					virtual bool write( base::data_t const & chunk ) = 0;
					virtual bool write( std::string chunk, base::Encoding const & encoding ) = 0;
					virtual StreamWritable& end( ) = 0;
					virtual StreamWritable& end( base::data_t const & chunk ) = 0;
					virtual StreamWritable& end( std::string chunk, base::Encoding const & encoding ) = 0;
					
					template<typename Listener>
					bool write( base::data_t const & chunk, Listener&& listener ) {
						return base::rollback_event_on_exception( this, "drain", std::forward<Listener>( listener ), [&]( ) {
							return write( chunk );
						} );
					}

					template<typename Listener>
					bool end( base::data_t const & chunk, Listener&& listener ) {
						return base::rollback_event_on_exception( this, "finish", std::forward<Listener>( listener ), [&]( ) {
							return end( chunk );
						} );
					}
					
				};	// class StreamWriteable

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
