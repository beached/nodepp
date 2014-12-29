#pragma once
#include <boost/utility/string_ref.hpp>
#include <boost/system/error_code.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <string>

namespace daw {
	namespace nodepp {
		namespace base {
			//////////////////////////////////////////////////////////////////////////
			// Summary:		Contains key/value pairs describing an error condition.
			//				Description is mandatory.
			// Requires:
			class Error {
				std::map<std::string, std::string> m_keyvalues;
				bool m_frozen;
				std::shared_ptr<Error> m_child;
				std::exception_ptr m_exception;
			public:
				Error( ) = delete;
				Error( boost::string_ref description );
				explicit Error( boost::system::error_code const & err );
				Error( Error && );
				Error( Error const & ) = default;
				Error( boost::string_ref description, std::exception_ptr ex_ptr );
				Error& operator=(Error const &) = default;
				Error& operator=(Error && rhs);

				Error& add( boost::string_ref name, boost::string_ref value );
				boost::string_ref get( boost::string_ref name ) const;
				std::string & get( boost::string_ref name );
				Error & child( ) const;
				bool has_child( ) const;
				Error& clear_child( );
				Error& child( Error child );
				void freeze( );
				bool has_exception( ) const;
				void throw_exception( );
				std::string to_string( boost::string_ref prefix = "" ) const;
			};	// class Error
			
			std::ostream& operator<<(std::ostream& os, Error const & error);

			using OptionalError = std::shared_ptr < Error > ;
			OptionalError create_optional_error( );
			
			template<typename... Args>
			OptionalError create_optional_error( Args&&... args ) {
				auto err = new Error( std::forward<Args>( args )... );
				return OptionalError( err );
			}
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
