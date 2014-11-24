#pragma once

#include <cstdint>
#include <string>
#include <memory>

#include "lib_event.h"
#include "lib_net_handle.h"
#include "lib_types.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			class Error;

			namespace net {
				class Address {

				};

				class SocketImpl;

				class Socket: public Handle {
					std::shared_ptr<SocketImpl> m_impl;
				public:
					using data_t = std::vector < uint8_t > ;

					struct events_t {
						CREATE_EVENT( connect );
						CREATE_EVENT( data, data_t );
						CREATE_EVENT( end );
						CREATE_EVENT( timeout );
						CREATE_EVENT( drain );
						CREATE_EVENT( error, Error );
						CREATE_EVENT( close, bool );
						enum class types { connect, data, end, timeout, drain, error, close };
						events_t( ) = default;
						~events_t( ) = default;
						events_t( events_t const & ) = delete;
						events_t& operator=( events_t const & ) = delete;
						events_t( events_t && ) = delete;
						events_t& operator=( events_t && ) = delete;
					};

					Socket( );
					Socket( options_t options );
					Socket( Socket const & ) = default;
					Socket& operator=(Socket const &) = default;
					Socket( Socket&& other );
					Socket& operator=(Socket&& rhs);
					~Socket( );

					events_t& events( );
					events_t const& events( ) const;

					Socket& connect( uint16_t port, std::string host, events_t::callback_t_connect callback = events_t::callback_t_connect{ } );
					Socket& connect( std::string path, events_t::callback_t_connect callback = events_t::callback_t_connect{ } );

					size_t& buffer_size( );
					size_t const & buffer_size( ) const;
					Socket& set_encoding( encoding_t encoding );

					bool write( data_t data, encoding_t const & encoding = "", events_t::callback_t_drain callback = events_t::callback_t_drain{ } );

					Socket& end( data_t data, encoding_t const & encoding = "" );

					Socket& destroy( );
					Socket& pause( );
					Socket& resume( );

					Socket& set_timeout( int32_t value, events_t::callback_t_timeout callback );
					Socket& set_no_delay( bool noDelay = true );
					Socket& set_keep_alive( bool keep_alive = false, int32_t initial_delay = 0 );
		
					Address const & address( ) const;	

					Socket& unref( );
					Socket& ref( );

					std::string remote_address( ) const;
					
					uint16_t local_port( ) const;
					
					size_t bytes_read( ) const;

					size_t bytes_written( ) const;
				};
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
