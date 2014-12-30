#include "lib_file.h"

#ifdef WIN32
#	define SUFFIX windows
#else
#	define SUFFIX generic
#endif

base::OptionalError read_file( boost::string_ref path, base::data_t & buffer, bool append_buffer ) {
	return read_file_SUFFIX( path, buffer, append_buffer );
}
void read_file_async( boost::string_ref path, std::function<void( base::OptionalError error, base::data_t data )> listener, std::shared_ptr<base::data_t> buffer, bool append_buffer ) {
	read_file_async_SUFFIX( path, std::move( listener ), std::move( buffer ), std::move( append_buffer ) );
}


std::streampos file_size( boost::string_ref path ) {
	return file_size_SUFFIX( path );
}

#undef  SUFFIX