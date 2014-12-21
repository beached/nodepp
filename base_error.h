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
				Error( std::string description );
				explicit Error( boost::system::error_code const & err );
				Error( Error && );
				Error( Error const & ) = default;
				Error( std::string description, std::exception_ptr ex_ptr );
				Error& operator=(Error const &) = default;
				Error& operator=(Error && rhs);

				Error& add( boost::string_ref name, std::string value );
				boost::string_ref get( boost::string_ref name ) const;
				std::string & get( boost::string_ref name );
				Error & child( ) const;
				bool has_child( ) const;
				Error& clear_child( );
				Error& child( Error child );
				void freeze( );
				bool has_exception( ) const;
				void throw_exception( );
				std::string to_string( std::string prefix = "" ) const;
			};	// class Error
			
			std::ostream& operator<<(std::ostream& os, Error const & error);
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
