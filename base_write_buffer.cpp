
#include "base_write_buffer.h"


namespace daw {
	namespace nodepp {
		namespace base {
			write_buffer::write_buffer( base::data_t const & source ) : buff( std::make_shared<base::data_t>( source ) ) { }

			write_buffer::write_buffer( boost::string_ref source ) : buff( std::make_shared<base::data_t>( source.begin( ), source.end( ) ) ) { }

			std::size_t write_buffer::size( ) const {
				return buff->size( );
			}

			write_buffer::data_type write_buffer::data( ) const {
				return buff->data( );
			}

			boost::asio::mutable_buffers_1 write_buffer::asio_buff( ) const {
				return boost::asio::buffer( data( ), size( ) );
			}
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
