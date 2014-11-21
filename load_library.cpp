#pragma once

#ifdef _WIN32
#include <windows.h>
#include <codecvt>
#include <locale>
#else
#include <dlfcn.h>
#endif

#include <memory>
#include <string>

#include "load_library.h"

namespace daw {
	namespace system {
#ifdef _WIN32
		std::wstring widen_string( std::string in_str ) {
			static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			return converter.from_bytes( in_str );
		}

		HINSTANCE load_library( std::wstring library_path ) {
			auto result = static_cast<HINSTANCE>(LoadLibraryW( library_path.c_str( ) ));
			if( !result ) {
				throw std::runtime_error( "Could not open library" );
			}
			return result;
		}

		HINSTANCE load_library( std::string library_path ) {
			return load_library( widen_string( std::move( library_path ) ) );
		}

		void close_library( HINSTANCE handle ) {
			if( nullptr != handle ) {
				FreeLibrary( handle );
			}
		}
#else
		void* load_library( std::string library_path ) {
			return dlopen( library_path.c_str( ), RTLD_LAZY );
		}

		void close_library( void* handle ) {
			if( nullptr != handle ) {
				dlclose( handle );
			}
		}
#endif
		LibraryHandle::LibraryHandle( ): m_handle{ nullptr } { }

		LibraryHandle::LibraryHandle( LibraryHandle && other ): m_handle( std::move( other.m_handle ) ) { }
		LibraryHandle& LibraryHandle::operator=(LibraryHandle&& rhs) {
			m_handle = std::move( rhs.m_handle );
			return *this;
		}
		LibraryHandle::~LibraryHandle( ) {
			close_library( m_handle );
		}
	}	// namespace system
}	// namespace daw
