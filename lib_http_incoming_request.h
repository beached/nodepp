#pragma once

#include "base_stream.h"
#include "lib_http_headers.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;

				enum class HttpRequestMethod { GET, HEAD, POST, PUT, DELETE, CONNECT, OPTIONS, TRACE };

				class HttpVersion {
					std::pair<uint8_t, uint8_t> m_version;
				public:
					uint8_t const & major( ) const;
					uint8_t & major( );
					uint8_t const & minor( ) const;
					uint8_t & minor( );

					HttpVersion( HttpVersion const & ) = default;
					HttpVersion& operator=( HttpVersion const & ) = default;
					~HttpVersion( ) = default;

					HttpVersion( );
					HttpVersion( uint8_t const & Major, uint8_t const & Minor );
					HttpVersion( std::string const & version );
					HttpVersion& operator=(std::string const & version);
					std::string to_string( ) const;
					operator std::string( ) const;
				};

				class HttpIncomingRequest: public base::stream::StreamReadable {
					
					HttpHeaders m_headers;
					HttpRequestMethod m_request_method;
					uint8_t m_http_version_major;
					uint8_t m_http_version_minor;

				public:
					HttpHeaders const & headers( ) const;
					HttpHeaders & headers( );

					HttpRequestMethod const & method( ) const;
					HttpRequestMethod & method( );

					HttpHeaders const & trailers( ) const;
					HttpHeaders & trailers( );

					HttpVersion const & version( ) const;
					HttpVersion & version( );

					// StreamReadable interface
					std::vector<std::string> const & valid_events( ) const override;

					virtual base::data_t read( ) override;
					virtual base::data_t read( size_t bytes ) override;

					virtual HttpIncomingRequest& set_encoding( base::Encoding const & encoding ) override;
					virtual HttpIncomingRequest& resume( ) override;
					virtual HttpIncomingRequest& pause( ) override;
					virtual base::stream::StreamWritable& pipe( base::stream::StreamWritable& destination ) override;
					virtual base::stream::StreamWritable& pipe( base::stream::StreamWritable& destination, base::options_t options ) override;
					virtual HttpIncomingRequest& unpipe( base::stream::StreamWritable& destination ) override;
					virtual HttpIncomingRequest& unshift( base::data_t const & chunk ) override;
				};

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw


