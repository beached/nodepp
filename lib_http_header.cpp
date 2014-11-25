#include <string>
#include <vector>

#include "lib_http_headers.h"
#include "string.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				HttpHeader::HttpHeader( ) : name{ }, value{ } { }

				HttpHeader::HttpHeader( std::string Name, std::string Value ) : name( Name ), value( Value ) { }

				std::string HttpHeader::to_string( ) const {
					return daw::string::string_format( "{0}	: {1}", name, value );
				}
				
				bool HttpHeader::empty( ) const {
					return name.empty( );
				}
				
				Headers::Headers( ) : headers( ) { }
				Headers::Headers( std::initializer_list<HttpHeader> values ) : headers( std::begin( values ), std::end( values ) ) { }

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
