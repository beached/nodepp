// The MIT License (MIT)
//
// Copyright (c) 2014-2015 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <memory>
#include <ostream>
#include <sstream>
#include <string>

#include "daw_json_link.h"

namespace daw {
	namespace json {
		JsonLink::JsonLink( std::string name ) :
			m_name( std::move( name ) ),
			m_data_map( ) { }

		JsonLink::JsonLink( JsonLink && other ):
			m_name( std::move( other.m_name ) ),
			m_data_map( std::move( other.m_data_map ) ) { }

		JsonLink& JsonLink::operator=(JsonLink && rhs) {
			if( this != &rhs ) {
				m_name = std::move( rhs.m_name );
				m_data_map = std::move( rhs.m_data_map );
			}
			return *this;
		}

		std::string & JsonLink::json_object_name( ) {
			return m_name;
		}
		std::string const & JsonLink::json_object_name( ) const {
			return m_name;
		}

		void JsonLink::reset_jsonlink( ) {
			m_data_map.clear( );
			m_name.clear( );
		}

		std::string JsonLink::encode( ) const {
			std::stringstream result;
			std::string tmp;
			auto is_first = true;
			for( auto const & value : m_data_map ) {
				value.second.encode( tmp );
				result << (!is_first ? ", " : "") << tmp;
				is_first = false;
			}
			return details::json_name( m_name ) + details::enbrace( result.str( ) );
		}

		void JsonLink::decode( boost::string_ref const json_text ) {
			decode( parse_json( json_text ) );
		}

		void JsonLink::decode( json_obj const & json_values ) {
			for( auto & value : m_data_map ) {
				value.second.decode( json_values );
			}
		}

		std::ostream& operator<<(std::ostream& os, JsonLink const & data) {
			os << data.encode( );
			return os;
		}

		void json_to_value( JsonLink & to, impl::value_t const & from ) {
			auto val = from;
			to.decode( std::make_shared<impl::value_t>( std::move( val ) ) );
		}

		std::string value_to_json( std::string const & name, JsonLink const & obj ) {
			return details::json_name( name ) + obj.encode( );
		}
	}	// namespace json
}	// namespace daw