#pragma once

#include <cstdint>
#include <string>
#include <memory>

#include "lib_net_handle.h"
#include "lib_types.h"
#include "lib_event_emitter.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			class Error;

			namespace net {
				class Address {

				};

				enum class SocketEvents { connect, data, end, timeout, drain, error, close, newListener, removeListener };
				class Socket: public Handle, virtual public daw::nodepp::base::generic::EventEmitter<SocketEvents> {
				public:
					using data_t = std::vector < uint8_t > ;
					
					Socket( );
					Socket( options_t options );
					Socket( Socket const & ) = default;
					Socket& operator=(Socket const &) = default;
					Socket( Socket&& other );
					Socket& operator=(Socket&& rhs);
					virtual ~Socket( );

					Socket& connect( uint16_t port, std::string host );

					template<typename Listener>
					Socket& connect( uint16_t port, std::string host, Listener listener ) {
						return this->rollback_event_on_exception( SocketEvents::connect, listener, [&]( ) {
							return connect( port, host );
						} );
					}

					Socket& connect( std::string path );

					template<typename Listener>
					Socket& connect( std::string path, Listener listener ) {
						return this->rollback_event_on_exception( SocketEvents::connect, listener, [&]( ) {
							return connect( path );
						} );
					}

					size_t& buffer_size( );
					size_t const & buffer_size( ) const;
					Socket& set_encoding( encoding_t encoding );

					bool write( data_t data, encoding_t const & encoding = "" );

					template<typename Listener>
					bool write( data_t data, encoding_t const & encoding = "", Listener listener ) {
						return this->rollback_event_on_exception( SocketEvents::drain, listener, [&]( ) {
							return write( data, encoding );
						} );
					}

					Socket& end( data_t data, encoding_t const & encoding = "" );

					Socket& destroy( );
					Socket& pause( );
					Socket& resume( );

					Socket& set_timeout( int32_t value );

					template<typename Listener>
					Socket& set_timeout( int32_t value, Listener listener ) {
						return this->rollback_event_on_exception( SocketEvents::timeout, listener, [&]( ) {
							set_timeout( value );
						} );
					}

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
