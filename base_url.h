

#include <boost/optional.hpp>
#include <map>
#include <string>

#include "base_types.h"

namespace daw {
	namespace nodepp {
		namespace base {

			struct Url {
				typedef std::map< std::string, std::string > query_t;
				boost::optional< std::string > root;
				boost::optional< std::string > hierarchy;
				boost::optional< query_t > queries;
			};


			std::shared_ptr<Url> parse_url( daw::nodepp::base::data_t::iterator first, daw::nodepp::base::data_t::iterator last );

		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
