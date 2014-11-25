#include <boost/shared_ptr.hpp>
#include <cinttypes>
#include <string>
#include <utility>

#include "base_event_emitter.h"
#include "lib_http_server.h"
#include "lib_net_server.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				Server::Server( ) : daw::nodepp::lib::net::Server{ } { }

				Server::Server( Server&& other ) : daw::nodepp::lib::net::Server{ std::move( other ) } { }

				Server& Server::operator=(Server&& rhs) { 
					if( this != &rhs ) {

					}
					return *this;
				}
				
				Server::~Server( ) {  }				

				bool Server::event_is_valid( std::string const & event ) const {
					static std::vector<std::string> const valid_events = { "request", "connection", "close", "checkContinue", "connect", "upgrade", "clientError", "listening", "newListener", "removeListener" };
					return daw::algorithm::find( valid_events, event ) != valid_events.end( ) || daw::nodepp::lib::net::Server::event_is_valid( event );
				}

				Server& Server::listen( uint16_t port, std::string hostname, uint16_t backlog ) { throw std::runtime_error( "Method Not Implemented" ); }

				Server& Server::listen( std::string path ) { throw std::runtime_error( "Method Not Implemented" ); }

				Server& Server::listen( daw::nodepp::lib::net::Handle& handle ) { throw std::runtime_error( "Method Not Implemented" ); }


				size_t& Server::max_header_count( ) { throw std::runtime_error( "Method Not Implemented" ); }
				size_t const & Server::max_header_count( ) const { throw std::runtime_error( "Method Not Implemented" ); }

				size_t Server::timeout( ) const { throw std::runtime_error( "Method Not Implemented" ); }


			}
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
