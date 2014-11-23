
#include <memory>
#include <string>

#include "load_library.h"

namespace daw {
	namespace system {
		namespace impl {
			OSLibraryHandle::OSLibraryHandle(std::string library_path) : m_handle{ impl::load_library(std::move(library_path)) } { 
				m_handle.set_cleaner( []( handle_t* handle ) {
					close_library( *handle );
				} );
			}
			OSLibraryHandle::OSLibraryHandle(std::wstring library_path) : m_handle{ impl::load_library(std::move(library_path)) } { 
				m_handle.set_cleaner( []( handle_t* handle ) {
					close_library( *handle );
				} );
			}

			OSLibraryHandle::OSLibraryHandle( OSLibraryHandle const & other ):m_handle( other.m_handle ) { }

			OSLibraryHandle::OSLibraryHandle( OSLibraryHandle && other ): m_handle( std::move( other.m_handle ) ) { }

			OSLibraryHandle& OSLibraryHandle::operator=(OSLibraryHandle rhs) {
				m_handle = std::move( rhs.m_handle );
				return *this;
			}
			
			OSLibraryHandle::handle_t& OSLibraryHandle::get( ) {
				return m_handle( );
			}

			OSLibraryHandle::handle_t const& OSLibraryHandle::get( ) const {
				return m_handle( );
			}

			OSLibraryHandle::~OSLibraryHandle( ) { }
		} // namespace impl

// 		LibraryHandle::LibraryHandle( LibraryHandle && other ): m_handle( std::move( other.m_handle ) ) { }
// 
// 		LibraryHandle& LibraryHandle::operator=(LibraryHandle rhs) {
// 			m_handle = std::move( rhs.m_handle );
// 			return *this;
// 		}


	}	// namespace system
}	// namespace daw
