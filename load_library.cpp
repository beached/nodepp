
#include <memory>
#include <string>

#include "load_library.h"

namespace daw {
	namespace system {
		namespace impl {
			OSLibraryHandle::OSLibraryHandle(std::string library_path) : m_handle{ impl::load_library(std::move(library_path)) }, m_count{ std::make_shared<size_t>(0) } { }
			OSLibraryHandle::OSLibraryHandle(std::wstring library_path) : m_handle{ impl::load_library(std::move(library_path)) }, m_count{ std::make_shared<size_t>(0) } { }


			OSLibraryHandle::OSLibraryHandle( OSLibraryHandle const & other ):m_handle( other.m_handle ), m_count( other.m_count ) {
				++(*m_count);
			}

			OSLibraryHandle::OSLibraryHandle( OSLibraryHandle && other ): m_handle( other.m_handle ), m_count( std::move( other.m_count ) ) { }

			OSLibraryHandle& OSLibraryHandle::operator=(OSLibraryHandle rhs) {
				m_handle = std::move( rhs.m_handle );
				m_count = std::move( rhs.m_count );
				return *this;
			}
			
			OSLibraryHandle::handle_t& OSLibraryHandle::get( ) {
				return m_handle;
			}

			OSLibraryHandle::handle_t const& OSLibraryHandle::get( ) const {
				return m_handle;
			}

			OSLibraryHandle::~OSLibraryHandle( ) {
				--(*m_count);
				if( 0 == m_count && m_handle ) {
					close_library( m_handle );
				}
			}
		} // namespace impl

		LibraryHandle::LibraryHandle( LibraryHandle && other ): m_handle( std::move( other.m_handle ) ) { }

		LibraryHandle& LibraryHandle::operator=(LibraryHandle rhs) {
			m_handle = std::move( rhs.m_handle );
			return *this;
		}


	}	// namespace system
}	// namespace daw
