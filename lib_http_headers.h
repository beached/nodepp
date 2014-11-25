#pragma once

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
				struct Headers {
					std::vector<HttpHeader> headers;

					Headers( );
					Headers( std::initializer_list<HttpHeader> values );
				};
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
