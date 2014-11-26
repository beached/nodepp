//#include <boost/asio.hpp>
#include <cstdint>
#include <string>

#include "base_enoding.h"
#include "base_event_emitter.h"
#include "base_types.h"
#include "lib_net_handle.h"
#include "lib_net_socket.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				using namespace daw::nodepp::base;
				NetSocket::NetSocket( ): Stream{ } { }
				
				NetSocket::NetSocket( options_t options ) : Stream{ } { }

				NetSocket::NetSocket( NetSocket&& other ) : Stream{ std::move( other ) } { }

				NetSocket& NetSocket::operator=(NetSocket&& rhs) {
					if( this != &rhs ) {
					}
					return *this;
				}

				NetSocket::~NetSocket( ) { }

				NetSocket& NetSocket::connect( uint16_t port, std::string host ) { throw std::runtime_error( "Method not implemented" ); }

				NetSocket& NetSocket::connect( std::string path ) { throw std::runtime_error( "Method not implemented" ); }

				size_t& NetSocket::buffer_size( ) { throw std::runtime_error( "Method not implemented" ); }

				size_t const & NetSocket::buffer_size( ) const { throw std::runtime_error( "Method not implemented" ); }

				NetSocket& NetSocket::set_encoding( daw::nodepp::base::Encoding encoding ) { throw std::runtime_error( "Method not implemented" ); }

				bool NetSocket::write( NetSocket::data_t data, daw::nodepp::base::Encoding const & encoding ) { throw std::runtime_error( "Method not implemented" ); }

				NetSocket& NetSocket::end( data_t data, daw::nodepp::base::Encoding ) { throw std::runtime_error( "Method not implemented" ); }

				NetSocket& NetSocket::destroy( ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocket& NetSocket::pause( ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocket& NetSocket::resume( ) { throw std::runtime_error( "Method not implemented" ); }

				NetSocket& NetSocket::set_timeout( int32_t value ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocket& NetSocket::set_no_delay( bool no_delay ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocket& NetSocket::set_keep_alive( bool keep_alive, int32_t initial_delay ) { throw std::runtime_error( "Method not implemented" ); }

				daw::nodepp::lib::net::NetAddress const & NetSocket::address( ) const { throw std::runtime_error( "Method not implemented" ); }

				NetSocket& NetSocket::unref( ) { throw std::runtime_error( "Method not implemented" ); }
				NetSocket& NetSocket::ref( ) { throw std::runtime_error( "Method not implemented" ); }

				daw::nodepp::lib::net::NetAddress const &  NetSocket::remote_address( ) const { throw std::runtime_error( "Method not implemented" ); }
				daw::nodepp::lib::net::NetAddress const & NetSocket::local_address( ) const { throw std::runtime_error( "Method not implemented" ); }
				uint16_t NetSocket::remote_port( ) const { throw std::runtime_error( "Method not implemented" ); }
				uint16_t NetSocket::local_port( ) const { throw std::runtime_error( "Method not implemented" ); }
				

				size_t NetSocket::bytes_read( ) const { throw std::runtime_error( "Method not implemented" ); }

				size_t NetSocket::bytes_written( ) const { throw std::runtime_error( "Method not implemented" ); }
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
