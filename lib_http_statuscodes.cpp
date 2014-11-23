
#include <cstdint>
#include <map>
#include <string>

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				std::map<int16_t, std::string> const STATUS_CODES = {
					{ 404, "Not found" }
				};
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
