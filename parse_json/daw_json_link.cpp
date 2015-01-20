#include "daw_json_link.h"

namespace daw {
	namespace json {
		namespace schema {
			::daw::json::impl::value_t get_schema( ) {
				return ::daw::json::impl::value_t( nullptr );
			}

			::daw::json::impl::value_t get_schema( bool const & ) {
				return ::daw::json::impl::value_t( false );
			}

			::daw::json::impl::value_t get_schema( nullptr_t ) {
				return ::daw::json::impl::value_t( nullptr );
			}

			::daw::json::impl::value_t get_schema( std::string const & ) {
				return ::daw::json::impl::value_t( "" );
			}
		}	// namespace schema
	}	// namespace json
}	// namespace daw
