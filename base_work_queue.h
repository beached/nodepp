#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <thread>

#include "base_event_emitter.h"
#include "concurrent_queue.h"
#include "semaphore.h"

namespace daw {
	namespace nodepp {
		namespace base {
			namespace impl {
				class WorkQueueImpl;
			}	// namespace impl

			using WorkQueue = std::shared_ptr < impl::WorkQueueImpl > ;

			//WorkQueue create_work_queue( uint32_t max_workers = std::thread::hardware_concurrency( ), daw::nodepp::base::EventEmitter emitter = daw::nodepp::base::create_event_emitter( ) );

			namespace impl {				
				using namespace daw::nodepp;
				using namespace daw::nodepp::base;

				struct work_item_t {
					std::function<void( )> work_item;
					std::function<void( base::OptionalError )> on_completion;
					
					work_item_t( );
					work_item_t( std::function<void( )> WorkItem, std::function<void( base::OptionalError )> OnCompletion = nullptr );
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
					~WorkQueueImpl( ) = default;
					WorkQueueImpl( WorkQueueImpl const & ) = delete;
					WorkQueueImpl& operator=(WorkQueueImpl const &) = delete;

					EventEmitter& emitter( );

					void add_work_item( std::function<void( )> work_item, std::function<void( base::OptionalError )> on_completion = nullptr, bool auto_start = true );

					void run( );
					void stop( bool wait = true );

				};	// class WorkQueueImpl
			}	// namespace impl
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
