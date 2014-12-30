
#include <fstream>

#include "lib_file.h"
#include "base_work_queue.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace file {

				base::data_t read_file( boost::string_ref path ) {
					base::data_t result;
					{
						std::ifstream in_file( path.to_string( ), std::ifstream::ate | std::ifstream::binary );
						if( in_file ) {
							auto fsize = in_file.tellg( );
							result.resize( fsize );
							in_file.seekg( 0 );
							in_file.read( result.data( ), fsize );
						}
					}
					return result;
				}

				void read_file_async( std::string const & path, std::function<void( base::OptionalError error, std::shared_ptr<base::data_t> data )> listener ) {
					auto buffer = std::make_shared<base::data_t>( );

					base::CommonWorkQueue( )->add_work_item( [path, buffer]( int64_t ) {
						*buffer = read_file( path );
					}, [buffer, listener]( int64_t, base::OptionalError error ) {
						listener( error, std::move( buffer ) );
					}, false );
				}

			}	// namespace file
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
