#pragma once
#include <boost/utility/string_ref.hpp>
#include <string>
#include <vector>


namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				struct HttpHeader {
					std::string name;
					std::string value;

					HttpHeader( );
					HttpHeader( std::string Name, std::string Value );
					std::string to_string( ) const;
					bool empty( ) const;
				};

				struct HttpHeaders {
					std::vector<HttpHeader> headers;

					HttpHeaders( );
					HttpHeaders( std::initializer_list<HttpHeader> values );										
					~HttpHeaders( ) = default;
					HttpHeaders( HttpHeaders const & ) = default;
					HttpHeaders( HttpHeaders && other );
					HttpHeaders& operator=(HttpHeaders && rhs);
					std::vector<HttpHeader>::iterator find( boost::string_ref header_name );
					std::vector<HttpHeader>::const_iterator find( boost::string_ref header_name ) const;

					std::string const& operator[]( boost::string_ref header_name ) const;
					std::string & operator[]( boost::string_ref header_name );

					std::string const& at( boost::string_ref header_name ) const;
					std::string & at( boost::string_ref header_name );

					bool exits( boost::string_ref header_name ) const;
					std::string to_string( );

					HttpHeaders& add( std::string header_name, std::string header_value );
				};
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
