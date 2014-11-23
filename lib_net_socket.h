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
					using namespace daw::nodepp::lib;
				public:					
					using data_t = std::vector < uint8_t >;
					
					struct events {						
						daw::nodepp::base::Event<> connect;
						using connect_callback_t = typename connect::callback_t;
						
						daw::nodepp::base::Event<data_t> data;
						using data_callback_t = typename data::callback_t;

						daw::nodepp::base::Event<> end;
						using data_callback_t = typename end::callback_t;

						daw::nodepp::base::Event<> timeout;
						using timeout_callback_t = typename timeout::callback_t;

						daw::nodepp::base::Event<> drain;
						using drain_callback_t = typename drain::callback_t;

						daw::nodepp::base::Event<Error> error;
						using error_callback_t = typename error::callback_t;

						daw::nodepp::base::Event<bool> close;
						using error_callback_t = typename close::callback_t;
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
