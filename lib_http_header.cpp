#include <string>
#include <vector>

#include "lib_http_headers.h"
#include "string.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				Header::Header( ) : name{ }, value{ } { }

				Header::Header( std::string Name, std::string Value ) : name( Name ), value( Value ) { }

				std::string Header::to_string( ) const {
					return daw::string::string_format( "{0}	: {1}", name, value );
				}
				
				bool Header::empty( ) const {
					return name.empty( );
				}
				
				Headers::Headers( ) : headers( ) { }
				Headers::Headers( std::initializer_list<Header> values ) : headers( std::begin( values ), std::end( values ) ) { }

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
