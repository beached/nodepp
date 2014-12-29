
#include "base_work_queue.h"

namespace daw {
	namespace nodepp {
		namespace base {

			WorkQueue::WorkQueue( ) { }
			WorkQueue::WorkQueue( size_t max_workers ) { }

			void WorkQueue::add_work_item( std::function<void( )> work_item ) { }


			void WorkQueue::run( ) { }
			void WorkQueue::stop( ) { }

			EventEmitter& WorkQueue::emitter( );
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
