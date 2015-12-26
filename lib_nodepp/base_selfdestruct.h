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
				// These are static so that the owner has the life of the program, not the individual instances
				static std::list<std::shared_ptr<SelfDestructing<Derived>>> s_selfs;	
				static std::mutex s_mutex;

			public:
				SelfDestructing( ) = delete;
				explicit SelfDestructing( daw::nodepp::base::EventEmitter emitter ): 
					daw::nodepp::base::StandardEvents<Derived>( std::move( emitter ) ) { }

				virtual ~SelfDestructing( ) = default;
				SelfDestructing( SelfDestructing const & ) = default;
				SelfDestructing( SelfDestructing && ) = default;
				SelfDestructing & operator=( SelfDestructing const & ) = default;
				SelfDestructing & operator=( SelfDestructing && ) = default;

				void arm( boost::string_ref event ) {
					std::unique_lock<std::mutex> lock1( s_mutex );
					auto obj = this->get_ptr( );
					auto pos = s_selfs.insert( std::end( s_selfs ), obj );
					this->emitter( )->add_listener( event.to_string( ) + "_selfdestruct", [pos]( ) {
						std::unique_lock<std::mutex> lock2( s_mutex );
						s_selfs.erase( pos );
					}, true );
				}
			};	// class SelfDestructing

#ifdef __clang__ 
// Hide warning abount global constructor, need it here until I find a better way
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wglobal-constructors"
#endif	//__clang__ 
			template<typename Derived>
			std::list<std::shared_ptr<SelfDestructing<Derived>>> SelfDestructing<Derived>::s_selfs;

			template<typename Derived>
			std::mutex SelfDestructing<Derived>::s_mutex;
#ifdef __clang__ 
#pragma clang diagnostic pop
#endif	//__clang__ 
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw

