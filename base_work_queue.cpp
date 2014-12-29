#include <future>
#include <thread>

#include "base_error.h"
#include "base_work_queue.h"
#include "scope_guard.h"

namespace daw {
	namespace nodepp {
		namespace base {
			namespace impl {
				using namespace daw::nodepp;
				using namespace daw::nodepp::base;

				work_item_t::work_item_t( ) : work_item( nullptr ), on_completion( nullptr ) { }

				work_item_t::work_item_t( std::function<void( )> WorkItem, std::function<void( base::OptionalError )> OnCompletion ) :
					work_item( std::move( WorkItem ) ),
					on_completion( std::move( OnCompletion ) ) { }
				
				bool work_item_t::valid( ) const {
					return static_cast<bool>(work_item);
				}


				WorkQueueImpl::WorkQueueImpl( uint32_t max_workers, EventEmitter emitter ) :
					m_work_queue( ),
					m_emitter( std::move( emitter ) ),
					m_continue( false ),
					m_worker_count( ),
					m_max_workers( std::move( max_workers ) ),
					m_item_count( 0 ) { }

				EventEmitter& WorkQueueImpl::emitter( ) {
					return m_emitter;
				}

				void WorkQueueImpl::add_work_item( std::function<void( )> work_item, std::function<void( base::OptionalError )> on_completion, bool auto_start ) {
					m_work_queue.push( work_item_t( std::move( work_item ), std::move( on_completion ) ) );
					if( auto_start ) {
						run( );
					}
				}				
				
				namespace {
					void on_main_thread( std::function<void( )> action ) {
						base::ServiceHandle::get( ).post( std::move( action ) );
					}
				}	// namespace anonymous

				void WorkQueueImpl::worker( ) {
					work_item_t current_item;
					m_worker_count.inc_counter( );
					auto on_exit = daw::on_scope_exit([&]( ) {
						m_worker_count.dec_counter( );
					} );
					while( m_continue ) {
						m_work_queue.wait_and_pop( current_item );
						if( current_item.valid( ) ) {							
							try {
								current_item.work_item( );
								if( current_item.on_completion ) {
									auto handler = current_item.on_completion;
									on_main_thread( [handler]( ) {
										handler( create_optional_error( ) );
									} );
								}
							} catch( ... ) {
								OptionalError error = base::create_optional_error( "Error processing WorkQueue", std::current_exception( ) );
								error->add( "where", "WorkQueueImpl::worker#current_item( )" );
								auto handler = current_item.on_completion;
								on_main_thread( [handler, error]( ) {
									handler( std::move( error ) );
								} );
							}
						} else {
							throw std::runtime_error( "WorkQueueImpl::worker -> Invalid work_item in queue" );
						}
					}					
				}

				void WorkQueueImpl::run( ) { 
					m_continue = true;
					auto self = get_ptr( );
					for( auto n = 0; n < m_max_workers; ++n ) {
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

				void WorkQueueImpl::stop( bool wait ) { 
					m_continue = false;
					if( wait ) {
						m_worker_count.wait( );
					}
				}

			}	// namespace impl

			WorkQueue create_work_queue( uint32_t max_workers, EventEmitter emitter ) {
				auto wq = new impl::WorkQueueImpl( std::move( max_workers ), std::move( emitter ) );
				return WorkQueue( wq );
			}
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
