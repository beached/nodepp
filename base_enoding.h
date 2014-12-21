#pragma once
#include <boost/utility/string_ref.hpp>
#include <cstdint>
#include <string>
#include <vector>

namespace daw {
	namespace nodepp {
		namespace base {
			//////////////////////////////////////////////////////////////////////////
			// Summary:		Wraps around an string representing the encoding of
			//				binary data (e.g. utf8, binary, hex)
			// Requires:
			class Encoding {
				std::string m_encoding;
				static std::vector<std::string> const &	valid_enodings( );
			public:
				Encoding( );
				explicit Encoding( std::string encoding );	
				Encoding( Encoding const & ) = default;
				Encoding& operator=(Encoding const &) = default;
				Encoding( Encoding && other );
				Encoding& operator=(Encoding && rhs);
				Encoding& operator=(boost::string_ref rhs);

				~Encoding( ) = default;

				boost::string_ref operator()( ) const;
				void set( std::string encoding );
				static bool is_valid_encoding( std::string encoding );

				operator std::string( ) const {
					return m_encoding;
				}

				
			};	// class Encoding
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw


