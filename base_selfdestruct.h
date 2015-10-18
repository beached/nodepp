#pragma once

#include <list>
#include <mutex>
#include "base_event_emitter.h"
#include <boost/utility/string_ref.hpp>

namespace daw {
	namespace nodepp {
		namespace base {
			// Creates a class that will destruct after the event name passed to it is called(e.g. close/end) unless it is referenced elsewhere
			template<typename Derived>
			class SelfDestructing: public daw::nodepp::base::enable_shared <Derived>, public daw::nodepp::base::StandardEvents<Derived> {
			private:
				static std::list<std::shared_ptr<Derived>> s_selfs;
				static std::mutex s_mutex;

			public:
				SelfDestructing( boost::string_ref event ) {
					std::unique_lock<std::mutex> lock1( s_mutex );
					auto pos = s_selfs.insert( std::end( s_selfs ), this->get_ptr( ) );
					this->emitter( )->add_listener( event.to_string( ) + "_selfdestruct", [pos]( ) {
						std::unique_lock<std::mutex> lock2( s_mutex );
						auto self = *pos;
						s_selfs.erase( pos );
					}, true );
				}
			};	// class SelfDestructing
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
