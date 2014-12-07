#pragma once

#include <string>

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
				// Requires:	base::EventEmitter
				class HttpClientRequest: public base::stream::Stream {
				public:
					HttpClientRequest( );

					std::vector<std::string> const & valid_events( ) const override;
					


					void abort( );

					template<typename Listener>
					HttpClientRequest& on( std::string event, Listener listener ) {
						add_listener( event, listener );
						return *this;
					}

					template<typename Listener>
					HttpClientRequest& once( std::string event, Listener listener ) {
						add_listener( event, listener, true );
						return *this;
					}

					// StreamReadable Interface
					virtual base::data_t read( ) override;
					virtual base::data_t read( size_t bytes ) override;

					virtual HttpClientRequest& set_encoding( base::Encoding const & encoding ) override;
					virtual HttpClientRequest& resume( ) override;
					virtual HttpClientRequest& pause( ) override;
					virtual StreamWritable& pipe( StreamWritable& destination ) override;
					virtual StreamWritable& pipe( StreamWritable& destination, base::options_t options ) override;
					virtual HttpClientRequest& unpipe( StreamWritable& destination ) override;
					virtual HttpClientRequest& unshift( base::data_t const & chunk ) override;

					// StreamWritable Interface
					virtual HttpClientRequest& write( base::data_t const & chunk ) override;
					virtual HttpClientRequest& write( std::string const & chunk, base::Encoding const & encoding = base::Encoding( ) ) override;

					virtual HttpClientRequest& end( ) override;
					virtual HttpClientRequest& end( base::data_t const & chunk ) override;
					virtual HttpClientRequest& end( std::string const & chunk, base::Encoding const & encoding = base::Encoding( ) ) override;
				};	// class HttpClientRequest

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
