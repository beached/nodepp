#include "daw_json_link.h"

namespace daw {
	namespace json {
		namespace schema {
			using namespace ::daw::json::impl;

			value_t get_schema( boost::string_ref name ) {
				return make_type_obj( name, value_t( std::string( "null" ) ) );
			}

			value_t get_schema( boost::string_ref name, bool const & ) {
				return make_type_obj( name, value_t( std::string( "bool" ) ) );
			}

			value_t get_schema( boost::string_ref name, std::nullptr_t ) {
				return make_type_obj( name, value_t( std::string( "null" ) ) );
			}

			value_t get_schema( boost::string_ref name, std::string const & ) {
				return make_type_obj( name, value_t( std::string( "string" ) ) );
			}

			value_t make_type_obj( boost::string_ref name, value_t selected_type ) {
				object_value result;
				if( !name.empty( ) ) {
					result.push_back( make_object_value_item(range::create_char_range( "name" ), value_t( name ) ) );
				}
				result.push_back( make_object_value_item(range::create_char_range( "type" ), std::move( selected_type ) ) );

				return value_t( std::move( result ) );
			}
		}	// namespace schema
	}	// namespace json
}	// namespace daw
