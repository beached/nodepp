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

#include <cassert>
#include <ctime>
#include <iomanip>

#include "base_json.h"
#include "base_json_parser.h"

#ifndef WIN32
void localtime_s( struct tm* result, std::time_t const * source ) {
	localtime_r( source, result );
}
#endif

namespace daw {
	namespace nodepp {
		namespace base {
			namespace json {
				std::string ts_to_string( std::time_t const & timestamp, std::string format ) {
					char buffer[200];
					std::tm tm = { };
					localtime_s( &tm, &timestamp );
					auto count = std::strftime( buffer, 200, format.c_str( ), &tm );
					assert( count < 200 );
					return std::string( buffer, buffer + count + 1 );
				}

				std::string to_string( std::string const & value ) {
					return value;
				}

				std::string enquote( std::string const & value ) {
					return "\"" + value + "\"";
				}

				namespace details {
					std::string json_name( std::string const & name ) {
						if( !name.empty( ) ) {
							return enquote( name ) + ": ";
						}
						return std::string( );
					}

					std::string enbrace( std::string const & json_value ) {
						return "{ " + json_value + " }";
					}
				}

				// string
				std::string value_to_json( std::string const & name, std::string const & value ) {
					return details::json_name( name ) + enquote( value );
				}

				// bool
				std::string value_to_json( std::string const & name, bool value ) {
					return details::json_name( name ) + (value ? "true" : "false");
				}

				// null
				std::string value_to_json( std::string const & name ) {
					return details::json_name( name ) + "null";
				}

				// date -> actaually a string, but it javascript date format encodes the date
				// 				std::string value_to_json_timestamp( std::string const & name, std::time_t const & timestamp ) {
				// 					using daw::nodepp::base::json::to_string;
				// 					return details::json_name( name ) + enquote( to_string( timestamp ) );
				// 				}

				std::string value_to_json( std::string const & name, int const & value ) {
					return value_to_json_number( name, value );
				}

				std::string value_to_json( std::string const & name, unsigned int const & value ) {
					return value_to_json_number( name, value );
				}

				std::string value_to_json( std::string const & name, int64_t const & value ) {
					return value_to_json_number( name, value );
				}

				std::string value_to_json( std::string const & name, uint64_t const & value ) {
					return value_to_json_number( name, value );
				}

				std::string value_to_json( std::string const & name, double const & value ) {
					return value_to_json_number( name, value );
				}

				std::string value_to_json( std::string const & name, JsonLink const & obj ) {
					return details::json_name( name ) + obj.encode( );
				}

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

				// 				JsonLink& JsonLink::link_timestamp( std::string name, std::time_t& value ) {
				// 					std::time_t *value_ptr = &value;
				// 					m_data_map[name] = [value_ptr, name]( std::string & json_text, Action action ) {
				// 						assert( value_ptr != nullptr );
				// 						std::time_t& val = *value_ptr;
				// 						switch( action ) {
				// 						case Action::encode:
				// 							json_text = value_to_json_timestamp( name, val );
				// 							break;
				// 						case Action::decode:
				// 							json_to_value( json_text, val );
				// 							break;
				// 						}
				// 					};
				// 					return *this;
				// 				}
				//

				namespace details {
					// String
					void json_to_value( std::string & to, impl::value_t const & from ) {
						to = from.get_string( );
					}

					// Boolean
					void json_to_value( bool & to, impl::value_t const & from ) {
						to = from.get_boolean( );
					}

					// Number, integer
					void json_to_value( int64_t & to, impl::value_t const & from ) {
						to = from.get_integral( );
					}

					// Number, real
					void json_to_value( double & to, impl::value_t const & from ) {
						to = from.get_real( );
					}

					void json_to_value( float & to, impl::value_t const & from ) {
						to = static_cast<float>(from.get_real( ));
					}

					void json_to_value( JsonLink & to, impl::value_t const & from ) {
						auto val = from;
						to.decode( std::make_shared<impl::value_t>( std::move( val ) ) );
					}
				}	// namespace details
			}	// namespace json
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw