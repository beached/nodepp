// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <future>
#include <thread>
#include <iostream>
#include "base_error.h"
#include "base_work_queue.h"
#include <daw/scope_guard.h>
#include "base_service_handle.h"

namespace daw {
	namespace nodepp {
		namespace base {
			namespace impl {
				using namespace daw::nodepp;
				using namespace daw::nodepp::base;

				work_item_t::work_item_t( ): work_item( nullptr ), on_completion( nullptr ), task_id( 0 ) { }

				work_item_t::work_item_t( int64_t TaskId, std::function<void( int64_t )> WorkItem, std::function<void( int64_t, base::OptionalError )> OnCompletion ) :
					work_item( std::move( WorkItem ) ),
					on_completion( std::move( OnCompletion ) ),
					task_id( std::move( TaskId ) ) { }

				bool work_item_t::valid( ) const {
					return task_id> 0 && static_cast<bool>(work_item);
				}

				WorkQueueImpl::WorkQueueImpl( uint32_t max_workers, EventEmitter emitter ):
					daw::nodepp::base::StandardEvents<WorkQueueImpl>( std::move( emitter ) ),
					m_work_queue( ),					
					m_continue( false ),
					m_worker_count( ),
					m_max_workers( std::move( max_workers ) ),
					m_item_count( 1 ) { }

				int64_t  WorkQueueImpl::add_work_item( std::function<void( int64_t )> work_item, std::function<void( int64_t, base::OptionalError )> on_completion, bool auto_start ) {
					int64_t task_id = static_cast<int64_t>(m_item_count++);
					m_work_queue.push( work_item_t( task_id, std::move( work_item ), std::move( on_completion ) ) );
					if( auto_start ) {
						run( );
					}
					return task_id;
				}

				namespace {
					void on_main_thread( std::function<void( )> action ) {
						base::ServiceHandle::get( ).post( action );
					}
				}	// namespace anonymous

				void WorkQueueImpl::worker( ) {
					work_item_t current_item;
					m_worker_count.inc_counter( );
					auto on_exit = daw::on_scope_exit( [&]( ) {
						m_worker_count.dec_counter( );
					} );
					while( m_continue ) {
						m_work_queue.wait_and_pop( current_item );
						if( m_continue && current_item.valid( ) ) {
							try {
								current_item.work_item( current_item.task_id );
								if( static_cast<bool>(current_item.on_completion) ) {
									std::cout <<"posting completion of task: " <<current_item.task_id <<"\n";
									on_main_thread( [current_item]( ) mutable {
										current_item.on_completion( current_item.task_id, create_optional_error( ) );
									} );
								} else {
									std::cout <<"not posting completion of task: " <<current_item.task_id <<"\n";
								}
							} catch( ... ) {
								if( current_item.on_completion ) {
									OptionalError error = base::create_optional_error( "Error processing WorkQueue", std::current_exception( ) );
									error->add( "where", "WorkQueueImpl::worker#current_item( )" );
									on_main_thread( [current_item, error]( ) mutable {
										current_item.on_completion( current_item.task_id, std::move( error ) );
									} );
								}
							}
						} else {
							throw std::runtime_error( "WorkQueueImpl::worker -> Invalid work_item in queue" );
						}
					}
				}

				void WorkQueueImpl::run( ) {
					m_continue = true;
					m_work_queue.reset( );
					auto self = this->get_ptr( );
					for( auto n = 0; n <m_max_workers && m_worker_count.count( ) <m_max_workers; ++n ) {
						std::async( [self]( ) {
							while( self->m_continue ) {
								try {
									self->worker( );
								} catch( ... ) {
									auto ptr = std::current_exception( );
									auto obj = self->get_weak_ptr( );
									on_main_thread( [ptr, obj]( ) {
										if( !obj.expired( ) ) {
											obj.lock( )->emit_error( ptr, "Exception in work queue", "WorkQueueImpl::run" );
										}
									} );
								}
							}
						} );
					}
				}

				void WorkQueueImpl::wait( ) {
					m_worker_count.wait( );
				}

				bool WorkQueueImpl::wait( size_t timeout_ms ) {
					return m_worker_count.wait( timeout_ms );
				}

				void WorkQueueImpl::stop( bool should_wait ) {
					m_continue = false;
					m_work_queue.exit_all( );
					if( should_wait ) {
						wait( );
					}
				}

				bool WorkQueueImpl::stop( size_t timeout_ms ) {
					m_continue = false;
					m_work_queue.exit_all( );
					return wait( timeout_ms );
				}

				WorkQueueImpl::~WorkQueueImpl( ) {
					m_continue = false;
					m_work_queue.exit_all( );
					if( !m_worker_count.wait( 2000 ) ) {	// TODO: Figure out time to wait for background workers to degrade gracefully
						// TODO: What to do if timeout is reached?
					}
				}

				int64_t WorkQueueImpl::max_conncurrent( ) const {
					return m_max_workers;
				}
			}	// namespace impl

			WorkQueue create_work_queue( uint32_t max_workers, EventEmitter emitter ) {
				auto wq = new impl::WorkQueueImpl( std::move( max_workers ), std::move( emitter ) );
				return WorkQueue( wq );
			}

			WorkQueue CommonWorkQueue( ) {
				static auto work_queue = []( ) {
					auto result = create_work_queue( );
					result->run( );
					return result;
				}();
				return work_queue;
			}
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw

