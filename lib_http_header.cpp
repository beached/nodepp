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
					return daw::string::string_format( "{0}: {1}", name, value );
				}
				
				bool HttpHeader::empty( ) const {
					return name.empty( );
				}
				
				HttpHeaders::HttpHeaders( ) : headers( ) { }
				HttpHeaders::HttpHeaders( std::initializer_list<HttpHeader> values ) : headers( std::begin( values ), std::end( values ) ) { }

				std::vector<HttpHeader>::iterator HttpHeaders::find( std::string const & header_name ) {
					auto it = std::find_if( std::begin( headers ), std::end( headers ), [&header_name]( HttpHeader const & item ) {
						return 0 == header_name.compare( item.name );
					} );					
					return it;
				}

				std::vector<HttpHeader>::const_iterator HttpHeaders::find( std::string const & header_name ) const {
					auto it = std::find_if( std::begin( headers ), std::end( headers ), [&header_name]( HttpHeader const & item ) {
						return 0 == header_name.compare( item.name );
					} );
					return it;
				}


				std::string const& HttpHeaders::operator[]( std::string const & header_name ) const {
					return find( header_name )->value;
				}

				std::string & HttpHeaders::operator[]( std::string const & header_name ) {
					return find( header_name )->value;
				}

				std::string const& HttpHeaders::at( std::string const& header_name ) const {
					auto it = HttpHeaders::find( header_name );
					if( it != std::end( headers )  ) {
						return it->value;						
					}
					throw std::out_of_range( header_name + " is not a valid header" );
				}

				std::string & HttpHeaders::at( std::string const& header_name ) {
					auto it = HttpHeaders::find( header_name );
					if( it != std::end( headers ) ) {
						return it->value;
					}
					throw std::out_of_range( header_name + " is not a valid header" );
				}

				std::string HttpHeaders::to_string( ) {
					std::stringstream ss;
					for( auto const& header : headers ) {
						ss << header.to_string( ) << "\r\n";
					}
					return ss.str( );
				}

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
