#pragma once

#include <string>
#include <vector>


namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				struct Header {
					std::string name;
					std::string value;

					Header( );
					Header( std::string Name, std::string Value );
					std::string to_string( ) const;
					bool empty( ) const;
				};
				struct Headers {
					std::vector<Header> headers;

					Headers( );
					Headers( std::initializer_list<Header> values );
				};
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
