
#include <boost/utility/string_ref.hpp>
#include <cstdint>
#include <string>
#include <utility>

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;

				class HttpVersion {
					std::pair<uint_fast8_t, uint_fast8_t> m_version;
					bool m_is_valid;
				public:
					uint_fast8_t const & major( ) const;
					uint_fast8_t & major( );
					uint_fast8_t const & minor( ) const;
					uint_fast8_t & minor( );
					HttpVersion( HttpVersion const & ) = default;
					HttpVersion& operator=(HttpVersion const &) = default;
					HttpVersion& operator=(HttpVersion && rhs);
					~HttpVersion( ) = default;

					HttpVersion( );
					HttpVersion( uint_fast8_t Major, uint_fast8_t Minor );
					HttpVersion( boost::string_ref version );
					HttpVersion& operator=(boost::string_ref version);
					std::string to_string( ) const;
					operator std::string( ) const;
					bool is_valid( ) const;
				};
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
