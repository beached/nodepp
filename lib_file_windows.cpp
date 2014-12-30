#ifdef WIN32

#include <limits>
#include <windows.h>

#include "lib_file.h"
#include "utf8.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace file {
				namespace {
					LARGE_INTEGER file_size_windows( HANDLE hnd ) {
						LARGE_INTEGER result;
						result.QuadPart = 0;
						if( !GetFileSizeEx( hnd, &result ) ) {
							result.QuadPart = -1;
						}
						return result;
					}

					template<typename StringType>
					std::wstring to_wstring( StringType utf8_string ) {
						std::wstring result;
						utf8::utf8to16( utf8_string.begin( ), utf8_string.end( ), std::back_inserter( result.end( ) ) );
						return result;
					}
				}	// namespace anonymous

				std::streampos file_size_windows( boost::string_ref path ) {
					auto wpath = to_wstring( path );
					auto hnd = CreateFile( wpath.c_str( ), 0, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr );
					if( hnd == INVALID_HANDLE_VALUE ) {
						return -1;
					}
					return static_cast<std::streampos>(file_size_windows( hnd ).QuadPart);
				}

				base::OptionalError read_file_windows( boost::string_ref path, base::data_t & buffer, bool append_buffer ) {
					auto wpath = to_wstring( path );
					auto hnd = CreateFile( wpath.c_str( ), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr );
					if( hnd == INVALID_HANDLE_VALUE ) {
						auto result = base::create_optional_error( "Could not open file" );
						result->add( "where", "read_file#CreateFile" );
						result->add( "win32_lasterror", std::to_string( GetLastError( ) ) );
						return result;
					}
					auto fsize = file_size_windows( hnd ).QuadPart;
					auto first_pos = append_buffer ? buffer.size( ) : 0;
					if( fsize < 0 ) {
						auto result = base::create_optional_error( "Determine file size" );
						result->add( "where", "read_file#file_size" );
						result->add( "win32_lasterror", std::to_string( GetLastError( ) ) );
						return result;
					} else if( fsize > static_cast<std::streampos>(std::numeric_limits<DWORD>::max( )) ) {
						return read_file_generic( path, buffer, append_buffer );	// TODO: implement large file read for WIN32
					} else if( fsize >= static_cast<std::streampos>( std::numeric_limits<size_t>::max( ) - first_pos ) ) {
						auto result = base::create_optional_error( "Cannot fit file into memory" );
						result->add( "memory_requested", std::to_string( fsize ) );
						result->add( "max_memory", std::to_string( std::numeric_limits<size_t>::max( ) ) );
						result->add( "where", "read_file#file_size" );
						return result;
					}
					buffer.resize( first_pos + fsize );
					DWORD bytes_read = 0;
					if( !ReadFile( hnd, static_cast<LPVOID>( buffer.data( ) + first_pos), fsize, &bytes_read, nullptr ) ) {
						auto result = base::create_optional_error( "Error reading file" );
						result->add( "where", "read_file#ReadFile" );
						result->add( "win32_lasterror", std::to_string( GetLastError( ) ) );
						return result;
					}
					return base::create_optional_error( );
				}

				namespace {
					struct overlapped_cb {
						OVERLAPPED overlapped;
						std::shared_ptr<base::data_t> buffer;
						std::function<void( base::OptionalError error, std::shared_ptr<base::data_t> data )> listener;
						overlapped_cb( ) : overlapped( 0 ), buffer( ), listener( nullptr ) { }
					};


				}	// namespace anonymous

				void read_file_async_windows( std::string const & path, std::function<void( base::OptionalError error, std::shared_ptr<base::data_t> data )> listener, std::shared_ptr<base::data_t> buffer, bool append_buffer ) {
					auto wpath = to_wstring( path );
					auto hnd = CreateFile( wpath.c_str( ), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr );
					if( hnd == INVALID_HANDLE_VALUE ) {
						auto result = base::Error( "Could not open file" );
						result.add( "where", "read_file#CreateFile" );
						result.add( "win32_lasterror", std::to_string( GetLastError( ) ) );
						throw result;
					}
					auto fsize = file_size_windows( hnd ).QuadPart;
					auto first_pos = append_buffer ? buffer->size( ) : 0;
					if( fsize < 0 ) {
						auto result = base::Error( "Determine file size" );
						result.add( "where", "read_file#file_size" );
						result.add( "win32_lasterror", std::to_string( GetLastError( ) ) );
						throw result;
					} else if( fsize > static_cast<std::streampos>(std::numeric_limits<DWORD>::max( )) ) {
						read_file_async_generic( path, listener, buffer, append_buffer );	// TODO: implement large file read for WIN32
						return;
					} else if( fsize >= static_cast<std::streampos>(std::numeric_limits<size_t>::max( ) - first_pos) ) {
						auto result = base::Error( "Cannot fit file into memory" );
						result.add( "memory_requested", std::to_string( fsize ) );
						result.add( "addressable_memory", std::to_string( std::numeric_limits<size_t>::max( ) ) );
						result.add( "where", "read_file#file_size" );
						throw result;
					}
					try {
						buffer->resize( first_pos + fsize );
					} catch( std::bad_alloc const & ba_ex ) {
						auto result = base::Error( "Cannot resize buffer", std::make_exception_ptr( ba_ex ) );
						result.add( "memory_requested", std::to_string( fsize ) );
						result.add( "addressable_memory", std::to_string( std::numeric_limits<size_t>::max( ) ) );
						result.add( "where", "read_file#buffer->resize" );
						throw result;
					}
					overlapped_cb ocb = { 0 };
					ocb.buffer = std::move( buffer );
					ocb.listener = std::move( listener );

					if( !ReadFileEx( hnd, static_cast<LPVOID>(ocb.buffer->data( ) + first_pos), fsize, &ocb.overlapped, []( DWORD error_no, DWORD bytes_copied, LPOVERLAPPED overlapped ) { 
						auto ocb = reinterpret_cast<overlapped_cb*>(overlapped);
						auto err = base::create_optional_error( );
						if( ERROR_SUCCESS != error_no ) {
							err = base::create_optional_error( "Error reading file" );
							err->add( "win32_lasterror", std::to_string( error_no ) );
							err->add( "bytes_copied", std::to_string( byt ) );
							err->add( "where", "read_file#ReadFileEx" );
						}


					} ) ) {
						auto result = base::Error( "Error reading file" );
						result.add( "where", "read_file#ReadFile" );
						result.add( "win32_lasterror", std::to_string( GetLastError( ) ) );
						throw result;
					}

				}

			}	// namespace file
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
#endif	// WIN32
