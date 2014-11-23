#pragma once

#include <cstdint>
#include <string>

#include "event_listener.h"
#include "lib_net_handle.h"
#include "lib_types.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			class Error;

			namespace net {
				class Address;

				class Socket: public Handle {					
				public:					
					using data_t = std::vector < uint8_t >;
					
					struct events {
						template<typename... Args>
						using event_t = daw::nodepp::base::Event < Args... > ;

						using event_t_connect = event_t< > ;
						event_t_connect connect;
						using connect_callback_t = event_t_connect::callback_t;

						using event_t_data = event_t<data_t> ;
						event_t_data data;
						using data_callback_t = event_t_data::callback_t;

						using event_t_end = event_t< > ;
						event_t_end end;
						using end_callback_t = event_t_end::callback_t;

						using event_t_timeout = event_t< > ;
						event_t_timeout timeout;
						using timeout_callback_t = event_t_timeout::callback_t;

						using event_t_drain = event_t < > ;
						event_t_drain drain;
						using drain_callback_t = event_t_drain::callback_t;

						using event_t_error = event_t<Error> ;
						event_t_error error;
						using error_callback_t = event_t_error::callback_t;

						using event_t_close = event_t<bool> ;
						event_t_close close;
						using close_callback_t = event_t_close::callback_t;

					};

					Socket( options_t options = options_t{ } );

					void connect( uint16_t port, std::string host, events::connect_callback_t callback = nullptr );
					void connect( std::string path, events::connect_callback_t callback = nullptr );

					size_t& buffer_size( );
					size_t const & buffer_size( ) const;
					void set_encoding( encoding_t encoding );					
					
					bool write( data_t data, encoding_t const & encoding = "", events::drain_callback_t callback = nullptr );

					void end( data_t data, encoding_t const & encoding = "" );

					void destroy( );
					void pause( );
					void resume( );

					void set_timeout( int32_t value, events::timeout_callback_t callback );
					void set_no_delay( bool noDelay = true );
					void set_keep_alive( bool keep_alive = false, int32_t initial_delay = 0 );
		
					Address address( ) const;	

					void unref( );
					void ref( );

					std::string remote_address( ) const;
					
					uint16_t local_port( ) const;
					
					size_t bytes_read( ) const;

					size_t bytes_written( ) const;
				};
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
