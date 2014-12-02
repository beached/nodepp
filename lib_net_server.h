#pragma once

#include <memory>
#include <string>

#include "base_error.h"
#include "base_event_emitter.h"
#include "base_types.h"
#include "lib_net_address.h"
#include "lib_net_socket_handle.h"
#include "base_service_handle.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {				
				//////////////////////////////////////////////////////////////////////////
				// Summary:		A TCP Server class
				// Requires:	base::EventEmitter, base::options_t,
				//				lib::net::NetAddress, base::Error
				class NetServer: public daw::nodepp::base::EventEmitter {
				public:
					NetServer( );
					NetServer( daw::nodepp::base::options_t options );
					NetServer( NetServer const & ) = default;
					NetServer& operator=(NetServer const &) = default;
					NetServer( NetServer&& other );
					NetServer& operator=(NetServer&& rhs);
					virtual ~NetServer( );
					
					virtual std::vector<std::string> const & valid_events( ) const override;

					NetServer& listen( uint16_t port, std::string hostname = "", uint16_t backlog = 511 );
					
					template<typename Listener>
					NetServer& listen( uint16_t port, std::string hostname, uint16_t backlog, Listener listener ) {
						return base::rollback_event_on_exception( this, "listening", listener, [&]( ) {
							return this->listen( port, hostname, backlog );
						} );
					}

					NetServer& listen( std::string socket_path );
					template<typename Listener>
					NetServer& listen( std::string socket_path, Listener listener ) {
						return base::rollback_event_on_exception( this, "listening", listener, [&]( ) {
							return listen( socket_path );
						} );
					}

					NetServer& listen( SocketHandle handle );
					template<typename Listener>
					NetServer& listen( SocketHandle handle, Listener listener ) {
						return base::rollback_event_on_exception( this, "listening", listener, [&]( ) {
							return listen( handle );
						} );
					}

					NetServer& close( );
					template<typename Listener>
					NetServer& close( Listener listener ) {
						return base::rollback_event_on_exception( this, "close", listener, [&]( ) {
							return close( );
						} );
					}

					daw::nodepp::lib::net::NetAddress const& address( ) const;
					NetServer& unref( );
					NetServer& ref( );
					NetServer& set_max_connections( uint16_t value );

					NetServer& get_connections( std::function<void( daw::nodepp::base::Error err, uint16_t count )> callback );


					template<typename Listener>
					NetServer& on( std::string event, Listener listener ) {
						add_listener( event, listener );
						return *this;
					}

					template<typename Listener>
					NetServer& once( std::string event, Listener listener ) {
						add_listener( event, listener, true );
						return *this;
					}
				};	// class server

			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
