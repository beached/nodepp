#pragma once

#include <boost/system/error_code.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

namespace daw {
	namespace nodepp {
		namespace base {
			//////////////////////////////////////////////////////////////////////////
			// Summary:		Contains key/value pairs describing an error condition.
			//				Description is mandatory.
			// Requires:
			class Error {
				std::unordered_map<std::string, std::string> m_keyvalues;
				std::unique_ptr<Error> m_child;
				bool m_frozen;
			public:
				Error( ) = delete;
				Error( Error const & ) = default;
				Error& operator=(Error const &) = default;

				Error( std::string description );
				Error( boost::system::error_code const & err );
				Error( Error && );
				Error& operator=(Error &&);
				Error& add( std::string name, std::string value );
				Error const &  get_child( ) const;
				bool has_child( ) const;
				Error& clear_child( );
				Error& set_child( Error const & child );
				void freeze( );

				std::string to_string( ) const;
			};	// class Error
			
			std::ostream& operator<<(std::ostream& os, Error const & error);
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
