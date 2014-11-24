#pragma once

#include <memory>
#include <string>

#include "lib_event.h"
#include "lib_net_handle.h"
#include "lib_types.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {				
				class Error;
				class Address;
				class ServerImpl;

				class Server: public Handle {
					std::shared_ptr<ServerImpl> m_impl;
				public:
					struct events_t {
						CREATE_EVENT( listening );
						CREATE_EVENT( connection );
						CREATE_EVENT( close );
						CREATE_EVENT( error );
						enum class types { listening, connection, close, error };
						events_t( ) = default;
						~events_t( ) = default;
						events_t( events_t const & ) = delete;
						events_t& operator=( events_t const & ) = delete;
						events_t( events_t && ) = delete;
						events_t& operator=( events_t && ) = delete;
					};

					Server( );
					Server( options_t options );
					Server( Server const & ) = default;
					Server& operator=(Server const &) = default;
					Server( Server&& other );
					Server& operator=(Server&& rhs);
					~Server( );

					events_t& events( );
					events_t const& events( ) const;


					Server& on( events_t::types event_type, events_t::callback_t_listening callback );
					Server& once( events_t::types event_type, events_t::callback_t_listening callback );

					Server& listen( uint16_t port, std::string hostname = "", uint16_t backlog = 511, events_t::callback_t_listening callback = events_t::callback_t_listening{ } );
					Server& listen( std::string socket_path, events_t::callback_t_listening callback = events_t::callback_t_listening{ } );
					Server& listen( Handle const & handle, events_t::callback_t_listening callback = events_t::callback_t_listening{ } );
					Server& close( events_t::callback_t_close callback = events_t::callback_t_close{ } );
					
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
