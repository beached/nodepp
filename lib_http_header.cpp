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
				
				HttpHeaders::HttpHeaders( ) : headers( ) { }
				HttpHeaders::HttpHeaders( std::initializer_list<HttpHeader> values ) : headers( std::begin( values ), std::end( values ) ) { }

				std::vector<HttpHeader>::iterator HttpHeaders::find( std::string const & name ) {
					auto it = std::find_if( std::begin( headers ), std::end( headers ), [&name]( HttpHeader const & item ) {
						return 0 == name.compare( item.name );
					} );					
					return it;
				}

				std::string const& HttpHeaders::operator[]( std::string const & name ) {
					return find( name )->value;
				}

				std::string const& HttpHeaders::at( std::string const& name ) {
					auto it = HttpHeaders::find( name );
					if( it != std::end( headers )  ) {
						return it->value;						
					}
					throw std::out_of_range( name + " is not a valid header" );
					
				}

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
