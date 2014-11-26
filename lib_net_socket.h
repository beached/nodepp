#pragma once
#include <boost/asio/ip/tcp.hpp>
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
				using namespace daw::nodepp;

				class NetSocket: public base::stream::Stream {
					std::shared_ptr<boost::asio::ip::tcp::socket> m_socket;
					std::shared_ptr<boost::asio::ip::tcp::endpoint> m_endpoint;
				public:
					
					NetSocket( );
					NetSocket( base::options_t options );
					NetSocket( NetSocket const & ) = default;
					NetSocket& operator=(NetSocket const &) = default;
					NetSocket( NetSocket&& other );
					NetSocket& operator=(NetSocket&& rhs);
					~NetSocket( );

					

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

					NetSocket& set_encoding( base::Encoding encoding );

					bool write( base::data_t data, base::Encoding const & encoding = base::Encoding{ } );					

					template<typename Listener>
					bool write( data_t data, base::Encoding const & encoding, Listener listener ) {
						return this->rollback_event_on_exception( SocketEvents::drain, listener, [&]( ) {
							return write( data, encoding );
						} );
					}

					NetSocket& end( base::data_t data, base::Encoding encoding = :base::Encoding{ } );

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
		
					lib::net::NetAddress const & address( ) const;	

					NetSocket& unref( );
					NetSocket& ref( );

					lib::net::NetAddress const & remote_address( ) const;
					lib::net::NetAddress const & local_address( ) const;
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
