#pragma once

#include <memory>
#include <string>
#ifdef _WIN32
#include "load_library_windows.h"
#else
#include "load_library_posix.h"
#endif

#include "utility.h"

namespace daw {
	namespace system {
		namespace impl {
			class OSLibraryHandle {
				using handle_t = decltype(load_library( "" ));
				handle_t m_handle;	
				std::shared_ptr<size_t> m_count;
			public:
				OSLibraryHandle( ) = delete;

				
				explicit OSLibraryHandle(std::string library_path);
				explicit OSLibraryHandle(std::wstring library_path);

				OSLibraryHandle( OSLibraryHandle const & other );
				OSLibraryHandle( OSLibraryHandle && other );
				OSLibraryHandle& operator=(OSLibraryHandle rhs);
				handle_t& get( );
				handle_t const& get( ) const;
					
				~OSLibraryHandle( );
			};	// class OSLibraryHandle
		}	// namespace impl

		class LibraryHandle {
			impl::OSLibraryHandle m_handle;
		public:
			//////////////////////////////////////////////////////////////////////////
			/// Summary: deleted
			LibraryHandle( ) = delete;

			template<typename StringType>
			LibraryHandle( StringType library_path ): m_handle{ std::move( library_path ) } { }

			LibraryHandle( LibraryHandle const & ) = default;
			LibraryHandle( LibraryHandle && other );
			LibraryHandle& operator=(LibraryHandle rhs);
			~LibraryHandle( ) = default;

			template<typename ResultType, typename... Args>
			ResultType call_function( std::string function_name, Args&&... function_args ) const {
				auto function_ptr = impl::get_function_address<ResultType, Args...>( m_handle.get( ), std::move( function_name ) );
				return (*function_ptr)(std::forward<Args>( function_args )...);
			}

			template<typename ResultType, typename... Args>
			auto get_function(std::string function_name) ->daw::function_pointer_t<ResultType, Args...> {
				auto function_ptr = impl::get_function_address<ResultType, Args...>(m_handle.get(), std::move(function_name));
				return function_ptr;
			}
		};	// class LibraryHandle

		template<typename ResultType, typename StringType, typename... Args>
		ResultType call_dll_function( StringType&& dll_name, std::string&& function_name, Args&&... function_args ) {
			auto lib = LibraryHandle( std::forward<StringType>( dll_name ) );
			auto result = lib.call_function<ResultType, Args...>( std::forward<std::string>( function_name ), std::forward<Args>( function_args )... );
			return result;
		}
	}	// namespace system
}	// namespace daw
