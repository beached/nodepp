
#include <cstdint>
#include <string>

#include "lib_event_emitter.h"
#include "lib_net_handle.h"
#include "lib_types.h"
#include "lib_net_socket.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				using namespace daw::nodepp::base::generic;
				Socket::Socket( ) :Handle{ }, EventEmitter < SocketEvents > { SocketEvents::removeListener, SocketEvents::newListener } { }
				
				Socket::Socket( options_t options ) : Handle{ }, EventEmitter < SocketEvents > { SocketEvents::removeListener, SocketEvents::newListener } { }

				Socket::Socket( Socket&& other ) : Handle{ /*TODO*/ }, EventEmitter<SocketEvents>{ other } { }

				Socket& Socket::operator=(Socket&& rhs) {
					if( this != &rhs ) {
					}
					return *this;
				}

				Socket::~Socket( ) { }

				Socket& Socket::connect( uint16_t port, std::string host ) { throw std::runtime_error( "Method not implemented" ); }

				Socket& Socket::connect( std::string path ) { throw std::runtime_error( "Method not implemented" ); }

				size_t& Socket::buffer_size( ) { throw std::runtime_error( "Method not implemented" ); }

				size_t const & Socket::buffer_size( ) const { throw std::runtime_error( "Method not implemented" ); }

				Socket& Socket::set_encoding( encoding_t encoding ) { throw std::runtime_error( "Method not implemented" ); }

				bool Socket::write( Socket::data_t data, encoding_t const & encoding ) { throw std::runtime_error( "Method not implemented" ); }

				Socket& Socket::end( data_t data, encoding_t const & encoding ) { throw std::runtime_error( "Method not implemented" ); }

				Socket& Socket::destroy( ) { throw std::runtime_error( "Method not implemented" ); }
				Socket& Socket::pause( ) { throw std::runtime_error( "Method not implemented" ); }
				Socket& Socket::resume( ) { throw std::runtime_error( "Method not implemented" ); }

				Socket& Socket::set_timeout( int32_t value ) { throw std::runtime_error( "Method not implemented" ); }
				Socket& Socket::set_no_delay( bool no_delay ) { throw std::runtime_error( "Method not implemented" ); }
				Socket& Socket::set_keep_alive( bool keep_alive, int32_t initial_delay ) { throw std::runtime_error( "Method not implemented" ); }

				Address const & Socket::address( ) const { throw std::runtime_error( "Method not implemented" ); }

				Socket& Socket::unref( ) { throw std::runtime_error( "Method not implemented" ); }
				Socket& Socket::ref( ) { throw std::runtime_error( "Method not implemented" ); }

				std::string Socket::remote_address( ) const { throw std::runtime_error( "Method not implemented" ); }

				uint16_t Socket::local_port( ) const { throw std::runtime_error( "Method not implemented" ); }

				size_t Socket::bytes_read( ) const { throw std::runtime_error( "Method not implemented" ); }

				size_t Socket::bytes_written( ) const { throw std::runtime_error( "Method not implemented" ); }
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
