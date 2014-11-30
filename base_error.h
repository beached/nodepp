#pragma once

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
			public:
				Error( ) = delete;
				Error( Error const & ) = default;
				Error& operator=(Error const &) = default;

				Error( std::string description );
				explicit Error( boost::system::error_code const & err );
				Error( Error && );
				Error& operator=(Error &&);
				Error& add( std::string const & name, std::string value );
				std::string const & get( std::string const & name ) const;
				std::string & get( std::string const & name );
				Error & get_child( ) const;
				bool has_child( ) const;
				Error& clear_child( );
				Error& set_child( Error const & child );
				void freeze( );

				std::string to_string( std::string prefix = "" ) const;
			};	// class Error
			
			std::ostream& operator<<(std::ostream& os, Error const & error);
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
