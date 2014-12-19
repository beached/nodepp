#pragma once

#include <cstdint>
#include <string>

#include "base_enoding.h"
#include "base_event_emitter.h"
#include "base_stream.h"
#include "base_types.h"
#include "base_enoding.h"
#include "lib_http.h"
#include "lib_http_headers.h"
#include "lib_http_version.h"
#include "lib_net_socket_stream.h"
#include "base_stream.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				class HttpServerResponseImpl;
				//////////////////////////////////////////////////////////////////////////
				// Summary:	Contains the data needed to respond to a client request				
				class HttpServerResponse: public base::stream::StreamWritable {
					std::shared_ptr<HttpServerResponseImpl> m_impl;
				public:
					HttpServerResponse( lib::net::NetSocketStream socket );
					HttpServerResponse( HttpServerResponse const & ) = default;
					virtual ~HttpServerResponse( ) = default;

					HttpServerResponse( HttpServerResponse && other );
					HttpServerResponse& operator=(HttpServerResponse rhs);

					virtual void write( base::data_t const & data ) override;
					virtual void write( std::string const & data, base::Encoding const & encoding = base::Encoding( ) ) override;
					virtual void end( ) override;
					virtual void end( base::data_t const & data ) override;
					virtual void end( std::string const & data, base::Encoding const & encoding = base::Encoding( ) ) override;

					void close( );

					HttpHeaders& headers( );
					HttpHeaders const & headers( ) const;

					void send_status( uint16_t status_code = 200 );
					void send_headers( );
					void send_body( );
					void clear_body( );
					bool send( );
					void reset( );
					bool is_open( );
					bool is_closed( ) const;
					bool can_write( ) const;

					void add_header( std::string header_name, std::string header_value );
				
					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when a write is completed
					/// Inherited from StreamWritable
					virtual void when_a_write_completes( std::function<void( )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when end( ... ) has been called and all data
					/// has been flushed
					/// Inherited from StreamWritable
					virtual void when_all_writes_complete( std::function<void( )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when end( ... ) has been called and all data
					/// has been flushed
					/// Inherited from StreamWritable
					virtual void when_next_all_writes_complete( std::function<void( )> listener ) override;

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when the next write is completed
					/// Inherited from StreamWritable
					virtual void when_next_write_completes( std::function<void( )> listener ) override;

					virtual void when_listener_added( std::function<void( std::string, base::Callback )> listener ) override;
					virtual void when_listener_removed( std::function<void( std::string, base::Callback )> listener ) override;
					virtual void when_error( std::function<void( base::Error )> listener ) override;

					virtual void when_next_listener_added( std::function<void( std::string, base::Callback )> listener ) override;
					virtual void when_next_listener_removed( std::function<void( std::string, base::Callback )> listener ) override;
					virtual void when_next_error( std::function<void( base::Error )> listener ) override;

				};	// struct ServerResponse			
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
