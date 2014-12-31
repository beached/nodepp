#include <fstream>

#include "lib_file.h"
#include "base_work_queue.h"
#include "utility.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace file {

				namespace {
					std::streampos file_size( std::ifstream & stream ) {
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

				std::streampos file_size( boost::string_ref path ) {
					std::ifstream in_file( path.to_string( ), std::ifstream::ate | std::ifstream::binary );
					return file_size( in_file );
				}				

				base::OptionalError read_file( boost::string_ref path, base::data_t & buffer, bool append_buffer ) {
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

				void read_file_async( boost::string_ref path, std::function<void( base::OptionalError error, std::shared_ptr<base::data_t> data )> callback, std::shared_ptr<base::data_t> buffer, bool append_buffer ) {
					base::CommonWorkQueue( )->add_work_item( [path, buffer, append_buffer]( int64_t ) mutable {
						if( !buffer ) {	// No existing buffer was supplied.  Create one
							buffer.reset( new base::data_t );
						} else if( !append_buffer ) {
							buffer->resize( 0 );
						}					
						if( auto err = read_file( path, *buffer ) ) {
							throw *err;
						}
					}, [buffer, callback]( int64_t, base::OptionalError error ) {
						callback( error, std::move( buffer ) );
					}, true );
				}

				base::OptionalError write_file( boost::string_ref path, base::data_t const & buffer, FileWriteMode mode, size_t bytes_to_write ) {
					// TODO: Write to a temp file first and then move 
					if( 0 == bytes_to_write || bytes_to_write > buffer.size( ) ) {
						bytes_to_write = buffer.size( );	// TODO: verify this is what we want.  Covers errors but that may be OK
					} 
					std::ofstream out_file;
					switch (mode) {
					case FileWriteMode::AppendOrCreate:
						out_file.open( path.to_string( ), std::ostream::binary | std::ostream::out | std::ostream::app );
						break;
					case FileWriteMode::MustBeNew: {
						if( std::ifstream( path.to_string( ) ) ) {
							// File exists.  Error
							auto error = base::create_optional_error( "Attempt to open an existing file when MustBeNew requested" );
							error->add( "where", "write_file" );
							return error;
						}
						out_file.open( path.to_string( ), std::ostream::binary | std::ostream::out );
						break;
					}												   
					case FileWriteMode::OverwriteOrCreate:
						out_file.open( path.to_string( ), std::ostream::binary | std::ostream::out | std::ostream::trunc );
						break;
					default:
						throw std::runtime_error( "Unknown FileWriteMode specified" );
					}
					if( !out_file ) {
						auto error = base::create_optional_error( "Could not open file for writing" );
						error->add( "where", "write_file#open" );
						return error;
					}
					if( !out_file.write( buffer.data( ), bytes_to_write ) ) {
						auto error = base::create_optional_error( "Error writing data to file" );
						error->add( "where", "write_file#write" );
						return error;
					}
					return base::create_optional_error( );
				}
				
				
				int64_t write_file_async( boost::string_ref path, base::data_t buffer, std::function<void( int64_t write_id, base::OptionalError error )> callback, FileWriteMode mode, size_t bytes_to_write ) {
					auto mbuffer = daw::as_move_capture( std::move( buffer ) );
					auto result = base::CommonWorkQueue( )->add_work_item( [path, mbuffer, mode, bytes_to_write]( int64_t ) mutable {
						if( auto err = write_file( path, mbuffer.value( ), mode, bytes_to_write ) ) {
							throw *err;
						}
					}, [callback]( int64_t write_id, base::OptionalError error ) {
						callback( std::move( write_id ), std::move( error ) );
					}, true );
					return result;
				}

			}	// namespace file
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
