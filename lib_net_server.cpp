
#include "lib_net_server.h"
#include "lib_event.h"
#include "lib_net_handle.h"
#include "lib_types.h"

#include <memory>
#include <utility>

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				using events_t = Server::events_t;

				class ServerImpl {
				public:
					events_t events;

					ServerImpl( ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					ServerImpl( options_t options ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					~ServerImpl( ) { }

					ServerImpl( ServerImpl const & ) = delete;
					ServerImpl& operator=(ServerImpl const &) = delete;
					ServerImpl( ServerImpl && ) = delete;
					ServerImpl& operator=(ServerImpl && rhs) = delete;


					void on( events_t::types event_type, events_t::callback_t_listening callback ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					void once( events_t::types event_type, events_t::callback_t_listening callback ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					void listen( uint16_t port, std::string hostname, uint16_t backlog, events_t::callback_t_listening callback ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					void listen( std::string socket_path, events_t::callback_t_listening callback ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					void listen( Handle const & handle, events_t::callback_t_listening callback ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					void close( events_t::callback_t_close callback ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					const Address& address( ) const {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					void unref( ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					void ref( ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					void set_max_connections( uint16_t value ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}


					void get_connections( std::function<void( Error err, uint16_t count )> callback ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}


				};	// class ServerImpl

				Server::Server( ) : m_impl{ std::make_shared<ServerImpl>( ) } { }
				Server::Server( options_t options ) : m_impl{ std::make_shared<ServerImpl>( options ) } { }
				Server::~Server( ) { }

				Server::Server( Server&& other ): m_impl( std::move( other.m_impl ) ) { }

				Server& Server::operator=(Server&& rhs) {
					if( this != &rhs ) {
						m_impl = std::move( rhs.m_impl );
					}
					return *this;
				}
				
				events_t& Server::events( ) { return m_impl->events; }
				events_t const& Server::events( ) const { return m_impl->events; }
				Server& Server::on( events_t::types event_type, events_t::callback_t_listening callback ) { m_impl->on( event_type, callback ); return *this; }
				Server& Server::once( events_t::types event_type, events_t::callback_t_listening callback ) { m_impl->once( event_type, callback ); return *this; }
				Server& Server::listen( uint16_t port, std::string hostname, uint16_t backlog, events_t::callback_t_listening callback ) { m_impl->listen( port, hostname, backlog, callback ); return *this; }
				Server& Server::listen( std::string socket_path, events_t::callback_t_listening callback ) { m_impl->listen( socket_path, callback ); return *this; }
				Server& Server::listen( Handle const & handle, events_t::callback_t_listening callback ) { m_impl->listen( handle, callback ); return *this; }
				Server& Server::close( events_t::callback_t_close callback ) { m_impl->close( callback ); return *this; }
				const Address& Server::address( ) const { return m_impl->address( ); }
				Server& Server::unref( ) { m_impl->unref( ); return *this; }
				Server& Server::ref( ) { m_impl->ref( ); return *this; }
				Server& Server::set_max_connections( uint16_t value ) { m_impl->set_max_connections( value ); return *this; }
				Server& Server::get_connections( std::function<void( Error err, uint16_t count )> callback ) { m_impl->get_connections( callback ); return *this; }
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
