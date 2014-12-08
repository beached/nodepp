#pragma once

#include "base_stream.h"
#include "base_url.h"
#include "lib_http_headers.h"
#include "lib_net_socket.h"
#include "lib_http_version.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;

				enum class HttpRequestMethod { Get, Post, Put, Head, Delete, Connect, Options, Trace, Unknown };

				class HttpIncomingRequest: public base::stream::StreamReadable {					
					HttpHeaders m_headers;
					HttpHeaders m_trailers;
					HttpRequestMethod m_method;					
					HttpVersion m_version;
					base::Url m_url;
					uint16_t m_status_code;
					std::shared_ptr<lib::net::NetSocket> m_socket;
				public:
					HttpIncomingRequest( ) = default;
					~HttpIncomingRequest( ) = default;
					HttpIncomingRequest( HttpIncomingRequest const & ) = default;
					HttpIncomingRequest& operator=(HttpIncomingRequest const &) = default;

					HttpHeaders const & headers( ) const;
					HttpHeaders & headers( );

					HttpRequestMethod const & method( ) const;
					HttpRequestMethod & method( );

					HttpHeaders const & trailers( ) const;
					HttpHeaders & trailers( );

					HttpVersion const & version( ) const;
					HttpVersion & version( );

					base::Url const & url( ) const;
					base::Url & url( );

					uint16_t& status( );
					uint16_t const & status( ) const;

					std::shared_ptr<lib::net::NetSocket> & socket( );
					std::shared_ptr<lib::net::NetSocket> const & socket( ) const;

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


