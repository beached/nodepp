#pragma once

//	The MIT License (MIT)
//	
//	Copyright (c) 2014-2015 Darrell Wright
//	
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//	
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//	
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
//


#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <thread>

#include "semaphore.h"
#include "base_event_emitter.h"
#include "concurrent_queue.h"

namespace daw {
	namespace nodepp {
		namespace base {
			namespace impl {
				class WorkQueueImpl;
			}	// namespace impl

			using WorkQueue = std::shared_ptr < impl::WorkQueueImpl > ;

			WorkQueue create_work_queue( uint32_t max_workers = std::thread::hardware_concurrency( ), daw::nodepp::base::EventEmitter emitter = daw::nodepp::base::create_event_emitter( ) );

			namespace impl {				
				using namespace daw::nodepp;
				using namespace daw::nodepp::base;

				struct work_item_t {
					std::function<void( int64_t )> work_item;
					std::function<void( int64_t, base::OptionalError )> on_completion;
					int64_t task_id;

					work_item_t( );
					work_item_t( int64_t task_id, std::function<void( int64_t )> WorkItem, std::function<void( int64_t, base::OptionalError )> OnCompletion = nullptr );
					work_item_t( work_item_t const & ) = default;
					work_item_t& operator=(work_item_t const &) = default;
					~work_item_t( ) = default;
					bool valid( ) const;
				};

				class WorkQueueImpl: public enable_shared<WorkQueueImpl>, public StandardEvents<WorkQueueImpl> {
					daw::concurrent_queue<work_item_t> m_work_queue;
					EventEmitter m_emitter;
					std::atomic<bool> m_continue;
					daw::thread::Semaphore<int> m_worker_count;
					int64_t m_max_workers;
					std::atomic<uint64_t> m_item_count;
					void worker( );

				public:
					friend WorkQueue create_work_queue( uint32_t, EventEmitter );
					//////////////////////////////////////////////////////////////////////////
					/// Summary Create a work queue with 1 worker per core
					WorkQueueImpl( uint32_t max_workers, EventEmitter emitter );
					~WorkQueueImpl( );
					WorkQueueImpl( WorkQueueImpl const & ) = delete;
					WorkQueueImpl& operator=(WorkQueueImpl const &) = delete;

					EventEmitter& emitter( );

					int64_t add_work_item( std::function<void( int64_t task_id )> work_item, std::function<void( int64_t task_id, base::OptionalError )> on_completion = nullptr, bool auto_start = true );

					void run( );
					void stop( bool should_wait = true );
					bool stop( size_t timeout_ms );
					void wait( );
					bool wait( size_t timeout_ms );
					int64_t max_conncurrent( ) const;
				};	// class WorkQueueImpl
			}	// namespace impl

			WorkQueue CommonWorkQueue( );

		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
