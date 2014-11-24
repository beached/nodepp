#pragma once

#include <memory>
#include <string>

#include "lib_event_emitter.h"
#include "lib_net_handle.h"
#include "lib_types.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {				
				class Error;
				class Address;

				enum class ServerEvents { listening, connection, close, error, newListener, remvoeListener };
				class Server: public Handle, virtual public daw::nodepp::base::generic::EventEmitter<ServerEvents> {
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
					Server& listen( uint16_t port, std::string hostname = "", uint16_t backlog = 511, Listener listener ) {
						return this->rollback_event_on_exception( ServerEvents::listening, listener, [&]( ) {
							return listen( port, hostname, backlog );
						} );
					}

					Server& listen( std::string socket_path );
					template<typename Listener>
					Server& listen( std::string socket_path, Listener listener ) {
						return this->rollback_event_on_exception( ServerEvents::listening, listener, [&]( ) {
							return listen( socket_path );
						} );
					}

					Server& listen( Handle const & handle );
					template<typename Listener>
					Server& listen( Handle const & handle, Listener listener ) {
						return this->rollback_event_on_exception( ServerEvents::listening, listener, [&]( ) {
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

				};	// class server

			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
