#include <boost/shared_ptr.hpp>
#include <cinttypes>
#include <string>
#include <utility>

#include "lib_http.h"
#include "lib_net_server.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp::base::generic;

				Server::Server( ) : EventEmitter < ServerEvents > { ServerEvents::newListener, ServerEvents::removeListener }  { }

				Server::Server( Server&& other ) : EventEmitter < ServerEvents > { std::move( other ) } { }

				Server& Server::operator=(Server&& rhs) { 
					if( this != &rhs ) {

					}
					return *this;
				}
				
				Server::~Server( ) {  }


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
