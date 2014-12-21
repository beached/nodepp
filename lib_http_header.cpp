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
					return name + ": " + value;
					//return daw::string::string_format( "{0}: {1}", name, value );
				}
				
				bool HttpHeader::empty( ) const {
					return name.empty( );
				}
				
				HttpHeaders::HttpHeaders( ) : headers( ) { }
				HttpHeaders::HttpHeaders( std::initializer_list<HttpHeader> values ) : headers( std::begin( values ), std::end( values ) ) { }

				HttpHeaders::HttpHeaders( HttpHeaders && other ) : headers( std::move( other.headers ) ) { }
				
				HttpHeaders& HttpHeaders::operator = (HttpHeaders && rhs) {
					if( this != &rhs ) {
						headers = std::move( rhs.headers );
					}
					return *this;
				}

				std::vector<HttpHeader>::iterator HttpHeaders::find( boost::string_ref header_name ) {
					auto it = std::find_if( std::begin( headers ), std::end( headers ), [&header_name]( HttpHeader const & item ) {
						return 0 == header_name.compare( item.name );
					} );					
					return it;
				}

				std::vector<HttpHeader>::const_iterator HttpHeaders::find( boost::string_ref header_name ) const {
					auto it = std::find_if( std::begin( headers ), std::end( headers ), [&header_name]( HttpHeader const & item ) {
						return 0 == header_name.compare( item.name );
					} );
					return it;
				}


				std::string const& HttpHeaders::operator[]( boost::string_ref header_name ) const {
					return find( header_name )->value;
				}

				std::string & HttpHeaders::operator[]( boost::string_ref header_name ) {
					auto it = find( header_name );
					if( it == headers.end( ) ) {
						it = headers.emplace( headers.end( ), header_name, "" );
					}
					return it->value;
				}

				bool HttpHeaders::exits( boost::string_ref header_name ) const {
					return find( header_name ) != headers.cend( );
				}

				std::string const& HttpHeaders::at( boost::string_ref header_name ) const {
					auto it = HttpHeaders::find( header_name );
					if( it != std::end( headers )  ) {
						return it->value;						
					}
					throw std::out_of_range( header_name.to_string( ) + " is not a valid header" );
				}

				std::string & HttpHeaders::at( boost::string_ref header_name ) {
					auto it = HttpHeaders::find( header_name );
					if( it != std::end( headers ) ) {
						return it->value;
					}
					throw std::out_of_range( header_name.to_string( ) + " is not a valid header" );
				}

				std::string HttpHeaders::to_string( ) {
					std::stringstream ss;
					for( auto const& header : headers ) {
						ss << header.to_string( ) << "\r\n";
					}
					return ss.str( );
				}

				HttpHeaders& HttpHeaders::add( std::string header_name, std::string header_value ) {
					headers.emplace_back( std::move( header_name ), std::move( header_value ) );
					return *this;
				}

			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
