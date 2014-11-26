#pragma once

#include <boost/any.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace daw {
	namespace nodepp {
		namespace base {
			using options_t = std::map < std::string, boost::any > ;
			using data_t = std::unique_ptr < std::vector < uint8_t > > ;
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw