#include <fstream>

#include "lib_file.h"
#include "base_work_queue.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace file {

				namespace {
					std::streampos file_size_generic( std::ifstream & stream ) {
						if( !stream ) {
							return -1;
						}
						auto current_pos = stream.tellg( );
						if( current_pos < 0 ) {
							return -1;
						}
						stream.seekg( 0, std::ifstream::end );
						auto result = stream.tellg( );
						stream.seekg( current_pos );
						return result;
					}
				}	// namespace anonymous

				std::streampos file_size_generic( boost::string_ref path ) {
					std::ifstream in_file( path.to_string( ), std::ifstream::ate | std::ifstream::binary );
					return file_size_generic( in_file );
				}

				base::OptionalError read_file_generic( boost::string_ref path, base::data_t & buffer, bool append_buffer ) {
					std::ifstream in_file( path.to_string( ), std::ifstream::ate | std::ifstream::binary );
					if( !in_file ) {
						auto result = base::create_optional_error( "Could not open file" );
						result->add( "where", "read_file#open" );
						return result;
					}
					auto fsize = in_file.tellg( );
					if( fsize < 0 ) {
						auto result = base::create_optional_error( "Error reading file length" );
						result->add( "where", "read_file#tellg" );
						return result;
					}
					if( !in_file.seekg( 0 ) ) {
						auto result = base::create_optional_error( "Error reseting file position to beginning" );
						result->add( "where", "read_file#seekg" );
						return result;
					}

					auto first_pos = append_buffer ? buffer.size( ) : 0;
					buffer.resize( first_pos + fsize );

					if( !in_file.read( buffer.data( ) + first_pos, fsize ) ) {
						auto result = base::create_optional_error( "Error reading file" );
						result->add( "where", "read_file#read" );
						return result;
					}
					return base::create_optional_error( );
				}

				void read_file_async_generic( std::string const & path, std::function<void( base::OptionalError error, std::shared_ptr<base::data_t> data )> listener, std::shared_ptr<base::data_t> buffer, bool append_buffer ) {
					base::CommonWorkQueue( )->add_work_item( [path, buffer, append_buffer]( int64_t ) mutable {
						if( !buffer ) {	// No existing buffer was supplied.  Create one
							buffer.reset( new base::data_t );
						} else if( !append_buffer ) {
							buffer->resize( 0 );
						}					
						if( auto err = read_file( path, *buffer ) ) {
							throw *err;
						}
					}, [buffer, listener]( int64_t, base::OptionalError error ) {
						listener( error, std::move( buffer ) );
					}, false );
				}

				base::OptionalError read_file( boost::string_ref path, base::data_t & buffer, bool append_buffer ) {
					return read_file_generic( path, buffer, append_buffer );
				}

				void read_file_async( boost::string_ref path, std::function<void( base::OptionalError error, std::shared_ptr<base::data_t> data )> listener, std::shared_ptr<base::data_t> buffer, bool append_buffer ) {
					read_file_async_generic( path.to_string(), std::move( listener ), std::move( buffer ), std::move( append_buffer ) );
				}


				std::streampos file_size( boost::string_ref path ) {
					return file_size_generic( path );
				}

			}	// namespace file
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
