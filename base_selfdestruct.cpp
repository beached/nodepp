#include "base_selfdestruct.h"
#include <list>
#include <mutex>

namespace daw {
	namespace nodepp {
		namespace base {
			template<typename Derived>
			std::list<std::shared_ptr<Derived>> SelfDestructing<Derived>::s_selfs;

			template<typename Derived>
			std::mutex SelfDestructing<Derived>::s_mutex;
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw