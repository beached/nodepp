

#include <boost/asio/buffer.hpp>
#include <boost/utility/string_ref.hpp>
#include <memory>

#include "base_types.h"

namespace daw {
	namespace nodepp {
		namespace base {

			struct write_buffer {
				using data_type = base::data_t::pointer;
				std::shared_ptr<base::data_t> buff;

				template<typename Iterator>
				write_buffer( Iterator first, Iterator last ) : buff( std::make_shared<base::data_t>( first, last ) ) { }

				write_buffer( base::data_t const & source );
				write_buffer( boost::string_ref source );
				std::size_t size( ) const;

				data_type data( ) const;
				boost::asio::mutable_buffers_1 asio_buff( ) const;
			};	// struct write_buffer

		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
