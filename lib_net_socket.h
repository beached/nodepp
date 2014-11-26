#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "base_enoding.h"
#include "base_stream.h"
#include "base_event_emitter.h"
#include "base_types.h"
#include "lib_net_address.h"
#include "lib_net_handle.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				class NetSocket: public daw::nodepp::base::stream::Stream {
				public:
					using data_t = std::vector < uint8_t > ;
					
					NetSocket( );
					NetSocket( daw::nodepp::base::options_t options );
					NetSocket( NetSocket const & ) = default;
					NetSocket& operator=(NetSocket const &) = default;
					NetSocket( NetSocket&& other );
					NetSocket& operator=(NetSocket&& rhs);
					virtual ~NetSocket( );

					

					NetSocket& connect( uint16_t port, std::string host );

					template<typename Listener>
					NetSocket& connect( uint16_t port, std::string host, Listener listener ) {
						return this->rollback_event_on_exception( SocketEvents::connect, listener, [&]( ) {
							return connect( port, host );
						} );
					}

					NetSocket& connect( std::string path );

					template<typename Listener>
					NetSocket& connect( std::string path, Listener listener ) {
						return this->rollback_event_on_exception( SocketEvents::connect, listener, [&]( ) {
							return connect( path );
						} );
					}

					size_t& buffer_size( );
					size_t const & buffer_size( ) const;

					NetSocket& set_encoding( daw::nodepp::base::Encoding encoding );

					bool write( data_t data, daw::nodepp::base::Encoding const & encoding = daw::nodepp::base::Encoding{ } );					

					template<typename Listener>
					bool write( data_t data, daw::nodepp::base::Encoding const & encoding, Listener listener ) {
						return this->rollback_event_on_exception( SocketEvents::drain, listener, [&]( ) {
							return write( data, encoding );
						} );
					}

					NetSocket& end( data_t data, daw::nodepp::base::Encoding encoding = daw::nodepp::base::Encoding{ } );

					NetSocket& destroy( );
					NetSocket& pause( );
					NetSocket& resume( );

					NetSocket& set_timeout( int32_t value );

					template<typename Listener>
					NetSocket& set_timeout( int32_t value, Listener listener ) {
						return this->rollback_event_on_exception( SocketEvents::timeout, listener, [&]( ) {
							set_timeout( value );
						} );
					}

					NetSocket& set_no_delay( bool noDelay = true );
					NetSocket& set_keep_alive( bool keep_alive = false, int32_t initial_delay = 0 );
		
					daw::nodepp::lib::net::NetAddress const & address( ) const;	

					NetSocket& unref( );
					NetSocket& ref( );

					daw::nodepp::lib::net::NetAddress const & remote_address( ) const;
					daw::nodepp::lib::net::NetAddress const & local_address( ) const;
					uint16_t remote_port( ) const;	
					uint16_t local_port( ) const;
					
					size_t bytes_read( ) const;
					size_t bytes_written( ) const;


					template<typename Listener>
					NetSocket& on( std::string event, Listener& listener ) {
						add_listener( event, listener );
						return *this;
					}

					template<typename Listener>
					NetSocket& once( std::string event, Listener& listener ) {
						add_listener( event, listener, true );
						return *this;
					}
				};
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
