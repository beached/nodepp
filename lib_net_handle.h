#pragma once

#include <map>
#include <boost/any.hpp>

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				class Handle {
				public:
					Handle( ) = default;
					Handle( Handle const & ) = default;
					Handle& operator=( Handle const & ) = default;
					virtual ~Handle( ) = default;
					//m_handle;
				};	// class Handle
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
