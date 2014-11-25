#pragma once

#include <memory>
#include <string>

#include "base_event_emitter.h"
#include "base_error.h"
#include "lib_net_address.h"
#include "lib_net_handle.h"
#include "lib_types.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {				
				
				class NetServer: public Handle, public daw::nodepp::base::EventEmitter {
				protected:
					virtual bool event_is_valid( std::string const & event ) const override;
				public:
					NetServer( );
					NetServer( options_t options );
					NetServer( NetServer const & ) = default;
					NetServer& operator=(NetServer const &) = default;
					NetServer( NetServer&& other );
					NetServer& operator=(NetServer&& rhs);
					virtual ~NetServer( );
					
					NetServer& listen( uint16_t port, std::string hostname = "", uint16_t backlog = 511 );
					
					template<typename Listener>
					NetServer& listen( uint16_t port, std::string hostname, uint16_t backlog, Listener listener ) {
						return this->rollback_event_on_exception( "listening", listener, [&]( ) {
							return this->listen( port, hostname, backlog );
						} );
					}

					NetServer& listen( std::string socket_path );
					template<typename Listener>
					NetServer& listen( std::string socket_path, Listener listener ) {
						return this->rollback_event_on_exception( "listening", listener, [&]( ) {
							return listen( socket_path );
						} );
					}

					NetServer& listen( Handle const & handle );
					template<typename Listener>
					NetServer& listen( Handle const & handle, Listener listener ) {
						return this->rollback_event_on_exception( "listening", listener, [&]( ) {
							return listen( handle );
						} );
					}

					NetServer& close( );
					template<typename Listener>
					NetServer& close( Listener listen ) {
						return this->rollback_event_on_exception( ServerEvents::close, listener, []( ) {
							return close( );
						} );
					}

					daw::nodepp::lib::net::NetAddress const& address( ) const;
					NetServer& unref( );
					NetServer& ref( );
					NetServer& set_max_connections( uint16_t value );

					NetServer& get_connections( std::function<void( daw::nodepp::base::Error err, uint16_t count )> callback );


					template<typename Listener>
					NetServer& on( std::string event, Listener& listener ) {
						add_listener( event, listener );
						return *this;
					}

					template<typename Listener>
					NetServer& once( std::string event, Listener& listener ) {
						add_listener( event, listener, true );
						return *this;
					}
				};	// class server

			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
