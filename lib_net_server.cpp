#include <memory>
#include <utility>

#include "lib_net_server.h"
#include "base_event_emitter.h"
#include "lib_net_handle.h"
#include "lib_types.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				using namespace daw::nodepp::base;
				Server::Server( ) :Handle{ }, EventEmitter{ } { }
				Server::Server( options_t options ) : Handle{ }, EventEmitter{ } { }
				Server::~Server( ) { }

				Server::Server( Server&& other ) : Handle{ std::move( other ) }, EventEmitter{ std::move( other ) } { }

				Server& Server::operator=(Server&& rhs) {
					if( this != &rhs ) {
						
					}
					return *this;
				}
				
				bool Server::event_is_valid( std::string const & event ) const {
					static std::vector<std::string> const valid_events = { "listening", "connection", "close", "error", "newListener", "remvoeListener" };
					return daw::algorithm::find( valid_events, event ) != valid_events.end( ) || EventEmitter::event_is_valid( event );
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
