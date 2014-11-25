#pragma once

#include <memory>
#include <string>

#include "base_event_emitter.h"
#include "lib_net_handle.h"
#include "lib_types.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {				
				class Error;
				class Address;

				
				class Server: public Handle, public daw::nodepp::base::EventEmitter {
				protected:
					virtual bool event_is_valid( std::string const & event ) const override;
				public:
					Server( );
					Server( options_t options );
					Server( Server const & ) = default;
					Server& operator=(Server const &) = default;
					Server( Server&& other );
					Server& operator=(Server&& rhs);
					virtual ~Server( );
					
					Server& listen( uint16_t port, std::string hostname = "", uint16_t backlog = 511 );
					
					template<typename Listener>
					Server& listen( uint16_t port, std::string hostname, uint16_t backlog, Listener listener ) {
						return this->rollback_event_on_exception( "listening", listener, [&]( ) {
							return this->listen( port, hostname, backlog );
						} );
					}

					Server& listen( std::string socket_path );
					template<typename Listener>
					Server& listen( std::string socket_path, Listener listener ) {
						return this->rollback_event_on_exception( "listening", listener, [&]( ) {
							return listen( socket_path );
						} );
					}

					Server& listen( Handle const & handle );
					template<typename Listener>
					Server& listen( Handle const & handle, Listener listener ) {
						return this->rollback_event_on_exception( "listening", listener, [&]( ) {
							return listen( handle );
						} );
					}

					Server& close( );
					template<typename Listener>
					Server& close( Listener listen ) {
						return this->rollback_event_on_exception( ServerEvents::close, listener, []( ) {
							return close( );
						} );
					}

					const Address& address( ) const;
					Server& unref( );
					Server& ref( );
					Server& set_max_connections( uint16_t value );

					Server& get_connections( std::function<void( Error err, uint16_t count )> callback );


					template<typename Listener>
					Server& on( std::string event, Listener& listener ) {
						add_listener( event, listener );
						return *this;
					}

					template<typename Listener>
					Server& once( std::string event, Listener& listener ) {
						add_listener( event, listener, true );
						return *this;
					}
				};	// class server

			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
