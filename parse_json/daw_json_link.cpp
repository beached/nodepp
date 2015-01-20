#include "daw_json_link.h"

namespace daw {
	namespace json {
		namespace schema {
			using namespace ::daw::json::impl;

			value_t get_schema( boost::string_ref name ) {
				return make_type_obj( name, value_t( "null" ) );
			}

			value_t get_schema( boost::string_ref name, bool const & ) {
				return make_type_obj( name, value_t( "bool" ) );
			}

			value_t get_schema( boost::string_ref name, std::nullptr_t ) {
				return make_type_obj( name, value_t( "null" ) );
			}

			value_t get_schema( boost::string_ref name, std::string const & ) {
				return make_type_obj( name, value_t( "string" ) );
			}

			value_t make_type_obj( boost::string_ref name, value_t selected_type ) {
				object_value result;
				if( !name.empty( ) ) {
					result.push_back( make_object_value_item( "name", value_t( name ) ) );
				}
				result.push_back( make_object_value_item( "type", std::move( selected_type ) ) );

				return value_t( std::move( result ) );
			}
		}	// namespace schema
	}	// namespace json
}	// namespace daw
