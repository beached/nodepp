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

		template<typename ResultType, typename... ArgTypes>
		auto get_function_address( const HINSTANCE& handle, std::string&& function_name ) -> typename std::add_pointer<ResultType( ArgTypes... )>::type {
			using function_ptr_t = std::add_pointer<ResultType( ArgTypes... )>::type;
			auto function_ptr = reinterpret_cast<function_ptr_t>(GetProcAddress( handle, function_name.c_str( ) ));
			if( !function_ptr ) {
				auto err = GetLastError( );
				throw std::runtime_error( "Could not load function in library" );
			}
			return function_ptr;
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

		template<typename ResultType, typename... ArgTypes>
		auto get_function_address( void* handle, std::string&& function_name ) -> typename std::add_pointer<ResultType( ArgTypes... )>::type {
			using factory_t = typename std::add_pointer<ResultType( ArgTypes... )>::type;
			dlerror( );
			auto function_ptr = reinterpret_cast<factory_t>( dlsym( handle, function_name.c_str( ) ) );
			auto has_error = dlerror( );
			if( has_error ) {
				throw std::runtime_error( "Could not load class factory in library" );
			}
			return function_ptr;
	}

#endif
		struct LibraryHandle {
			using handle_t = decltype(load_library( "" ));
			handle_t m_handle;
			LibraryHandle( ): m_handle{ nullptr } { }

			template<typename StringType>
			LibraryHandle( StringType library_path ): m_handle{ load_library( std::move( library_path ) ) } {
				if( !m_handle ) {
					throw std::runtime_error( "Error loading library" );
				}
			}

			LibraryHandle( LibraryHandle const & ) = delete;
			LibraryHandle& operator=(LibraryHandle const &) = delete;
			LibraryHandle( LibraryHandle && other ): m_handle( std::move( other.m_handle ) ) { }
			LibraryHandle& operator=(LibraryHandle&& rhs) {
				m_handle = std::move( rhs.m_handle );
				return *this;
			}
			~LibraryHandle( ) {
				close_library( m_handle );
			}

			template<typename ResultType, typename... Args>
			ResultType call_function( std::string function_name, Args&&... function_args ) const {
				auto function_ptr = get_function_address<ResultType, Args...>( m_handle, std::move( function_name ) );
				return (*function_ptr)(std::forward<Args>( function_args )...);
			}
		};


		template<typename ResultType, typename StringType, typename... Args>
		ResultType call_dll_function( StringType&& dll_name, std::string&& function_name, Args&&... function_args ) {
			auto lib = daw::system::LibraryHandle( std::forward<StringType>( dll_name ) );
			auto result = lib.call_function<ResultType, Args...>( std::forward<std::string>( function_name ), std::forward<Args>( function_args )... );
			return result;
		}
	}	// namespace system
}	// namespace daw
