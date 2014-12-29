#pragma once

#include <functional>
#include <list>

#include "base_event_emitter.h"
#include "semaphore.h"

namespace daw {
	namespace nodepp {
		namespace base {
			class WorkQueue: public StandardEvents < WorkQueue > {
				using work_list_t = std::list < std::function<void( )> > ;				

				work_list_t m_work_items;				
				EventEmitter m_emitter;
				bool m_continue;
				daw::thread::Semaphore<int> m_worker_count;
			public:
				//////////////////////////////////////////////////////////////////////////
				/// Summary: Create a work queue with 1 worker per core
				WorkQueue( );
				WorkQueue( size_t max_workers );
				~WorkQueue( ) = default;
				WorkQueue( WorkQueue const & ) = delete;
				WorkQueue& operator=(WorkQueue const &) = delete;

				void add_work_item( std::function<void( )> work_item );


				void run( );
				void stop( );

				EventEmitter& emitter( );
			};	// class WorkQueue
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
