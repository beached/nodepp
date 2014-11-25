#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "base_enoding.h"
#include "base_event_emitter.h"
#include "lib_net_handle.h"
#include "lib_types.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			class Error;

			namespace net {
				class Address {

				};

				class Socket: public Handle, public daw::nodepp::base::EventEmitter {
				protected:
					virtual bool event_is_valid( std::string const & event ) const override;
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
					Socket& set_encoding( daw::nodepp::base::Encoding encoding );

					bool write( data_t data, daw::nodepp::base::Encoding const & encoding = daw::nodepp::base::Encoding{ } );					

					template<typename Listener>
					bool write( data_t data, daw::nodepp::base::Encoding const & encoding, Listener listener ) {
						return this->rollback_event_on_exception( SocketEvents::drain, listener, [&]( ) {
							return write( data, encoding );
						} );
					}

					Socket& end( data_t data, daw::nodepp::base::Encoding encoding = daw::nodepp::base::Encoding{ } );

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


					template<typename Listener>
					Socket& on( std::string event, Listener& listener ) {
						add_listener( event, listener );
						return *this;
					}

					template<typename Listener>
					Socket& once( std::string event, Listener& listener ) {
						add_listener( event, listener, true );
						return *this;
					}
				};
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
