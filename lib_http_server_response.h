#pragma once

#include <string>
#include "base_event_emitter.h"
#include "lib_http_headers.h"
#include "base_enoding.h"
#include "base_stream.h"
#include "lib_http_chunk.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				//////////////////////////////////////////////////////////////////////////
				// Summary:		
				// Requires:	base::EventEmitter, lib::http::HttpHeader,
				//				lib::http::HttpHeaders, lib::http::HttpChunk,
				//				base::Encodingn
				class HttpServerResponse: public base::stream::StreamWritable {
					HttpServerResponse( );
					friend class HttpServer;
				public:
					

					HttpServerResponse& write_continue( );
					HttpServerResponse& write_head( uint16_t status_code, std::string reason_phrase = "", HttpHeaders headers = HttpHeaders{ } );
					
					template<typename Listener>
					HttpServerResponse& set_timeout( size_t msecs, Listener listener ) {
						throw std::runtime_error( "Method not implemented" );
					}
					
					uint16_t& status_code( );
					uint16_t const & status_code( ) const;

					void set_header( std::string name, std::string value );
					void set_header( HttpHeaders headers );

					bool headers_sent( ) const;

					bool& send_date( );
					bool const& send_date( ) const;

					HttpHeader const & get_header( std::string name ) const;
					HttpServerResponse& remove_header( std::string name );

					bool write_chunk( std::string const & chunk, base::Encoding const & encoding = base::Encoding{ } );
					bool write_chunk( base::data_t const & chunk, base::Encoding const & encoding = base::Encoding{ } );

					bool add_trailers( HttpHeaders headers );

					// StreamWriteable overrides
					virtual std::vector<std::string> const & valid_events( ) const override;
					virtual HttpServerResponse& write( base::data_t const & chunk ) override;
					virtual HttpServerResponse& write( std::string const & chunk, base::Encoding const & encoding = base::Encoding( ) ) override;
					virtual void end( ) override;
					virtual void end( base::data_t const & chunk ) override;
					virtual void end( std::string const & chunk, base::Encoding const & encoding = base::Encoding( ) ) override;

					// Event callbacks

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the stream is closed before end( ) was
					/// called or able to flush
					HttpServerResponse& on_close( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when an error occurs
					/// Inherited from EventEmitter
					virtual HttpServerResponse& on_error( std::function<void( base::Error )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the headers and message has been sent
					/// over the wire.  The data may/may not have been received yet.  This
					/// is the last event emitted on this object
					/// Inherited from StreamWritable
					HttpServerResponse& on_finish( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted whenever this StreamWritable is passed to 
					/// pipe( ) on a StreamReadable
					/// Inherited from StreamWritable (Not implemented yet)
					virtual HttpServerResponse& on_pipe( std::function<void( base::stream::StreamReadable& )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted whenever this StreamWritable is passed to 
					/// unpipe( ) on a StreamReadable
					/// Inherited from StreamWritable (Not implemented yet)
					virtual HttpServerResponse& on_unpipe( std::function<void( base::stream::StreamReadable& )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the stream is closed before end( ) was
					/// called or able to flush
					HttpServerResponse& once_close( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when an error occurs
					/// Inherited from EventEmitter
					virtual HttpServerResponse& once_error( std::function<void( base::Error )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the headers and message has been sent
					/// over the wire.  The data may/may not have been received yet.  This
					/// is the last event emitted on this object
					/// Inherited from StreamWritable
					HttpServerResponse& once_finish( std::function<void( )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted whenever this StreamWritable is passed to 
					/// pipe( ) on a StreamReadable
					/// Inherited from StreamWritable (Not implemented yet)
					virtual HttpServerResponse& once_pipe( std::function<void( base::stream::StreamReadable& )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted whenever this StreamWritable is passed to 
					/// unpipe( ) on a StreamReadable
					/// Inherited from StreamWritable (Not implemented yet)
					virtual HttpServerResponse& once_unpipe( std::function<void( base::stream::StreamReadable& )> listener ) override;


				};	// class ServerResponse			


			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
