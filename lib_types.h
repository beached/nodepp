#pragma once

#include <boost/any.hpp>
#include <map>
#include <string>

namespace daw {
	namespace nodepp {
		namespace lib {
			using options_t = std::map < std::string, boost::any > ;

		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
