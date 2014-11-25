#include <memory>
#include <utility>

#include "base_event_emitter.h"
#include "base_types.h"
#include "lib_net_handle.h"
#include "lib_net_server.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				using namespace daw::nodepp::base;
				NetServer::NetServer( ) :Handle{ }, EventEmitter{ } { }
				NetServer::NetServer( options_t options ) : Handle{ }, EventEmitter{ } { }
				NetServer::~NetServer( ) { }

				NetServer::NetServer( NetServer&& other ) : Handle{ std::move( other ) }, EventEmitter{ std::move( other ) } { }

				NetServer& NetServer::operator=(NetServer&& rhs) {
					if( this != &rhs ) {
						
					}
					return *this;
				}
				
				bool NetServer::event_is_valid( std::string const & event ) const {
					static std::vector<std::string> const valid_events = { "listening", "connection", "close", "error", "newListener", "remvoeListener" };
					return daw::algorithm::find( valid_events, event ) != valid_events.end( ) || EventEmitter::event_is_valid( event );
				}

				NetServer& NetServer::listen( uint16_t port, std::string hostname, uint16_t backlog ) { throw std::runtime_error( "Method Not Implemented" ); }
				NetServer& NetServer::listen( std::string socket_path ) { throw std::runtime_error( "Method Not Implemented" ); }
				NetServer& NetServer::listen( Handle const & handle ) { throw std::runtime_error( "Method Not Implemented" ); }
				NetServer& NetServer::close( ) { throw std::runtime_error( "Method Not Implemented" ); }
				daw::nodepp::lib::net::NetAddress const & NetServer::address( ) const { throw std::runtime_error( "Method Not Implemented" ); }
				NetServer& NetServer::unref( ) { throw std::runtime_error( "Method Not Implemented" ); }
				NetServer& NetServer::ref( ) { throw std::runtime_error( "Method Not Implemented" ); }
				NetServer& NetServer::set_max_connections( uint16_t value ) { throw std::runtime_error( "Method Not Implemented" ); }
				NetServer& NetServer::get_connections( std::function<void( Error err, uint16_t count )> callback ) { throw std::runtime_error( "Method Not Implemented" ); }
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
