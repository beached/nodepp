#pragma once

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
				bool m_frozen;
			public:
				Error( ) = delete;
				Error( Error const & ) = default;
				Error& operator=(Error const &) = default;

				Error( std::string description );
				Error( Error && );
				Error& operator=(Error &&);
				Error& add( std::string name, std::string value );
				void freeze( );
			};	// class Error
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
