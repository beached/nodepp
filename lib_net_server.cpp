
#include "lib_net_server.h"
#include "lib_event_emitter.h"
#include "lib_net_handle.h"
#include "lib_types.h"

#include <memory>
#include <utility>

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				using namespace daw::nodepp::base::generic;
				Server::Server( ) :Handle{ }, EventEmitter < ServerEvents > { ServerEvents::remvoeListener, ServerEvents::newListener } { }
				Server::Server( options_t options ) : Handle{ }, EventEmitter < ServerEvents > { ServerEvents::remvoeListener, ServerEvents::newListener } { }
				Server::~Server( ) { }

				Server::Server( Server&& other ) : Handle{ /*TODO*/ }, EventEmitter < ServerEvents > { other } { }

				Server& Server::operator=(Server&& rhs) {
					if( this != &rhs ) {
						
					}
					return *this;
				}

				Server& Server::listen( uint16_t port, std::string hostname, uint16_t backlog ) { throw std::runtime_error( "Method Not Implemented" ); }
				Server& Server::listen( std::string socket_path ) { throw std::runtime_error( "Method Not Implemented" ); }
				Server& Server::listen( Handle const & handle ) { throw std::runtime_error( "Method Not Implemented" ); }
				Server& Server::close( ) { throw std::runtime_error( "Method Not Implemented" ); }
				Address const & Server::address( ) const { throw std::runtime_error( "Method Not Implemented" ); }
				Server& Server::unref( ) { throw std::runtime_error( "Method Not Implemented" ); }
				Server& Server::ref( ) { throw std::runtime_error( "Method Not Implemented" ); }
				Server& Server::set_max_connections( uint16_t value ) { throw std::runtime_error( "Method Not Implemented" ); }
				Server& Server::get_connections( std::function<void( Error err, uint16_t count )> callback ) { throw std::runtime_error( "Method Not Implemented" ); }
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
