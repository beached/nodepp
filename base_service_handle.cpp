#include <boost/asio/io_service.hpp>
#include <future>

#include "base_service_handle.h"


namespace daw {
	namespace nodepp {
		namespace base {
			boost::asio::io_service& ServiceHandle::get( ) {
				static boost::asio::io_service result;
				return result;
			}

			void ServiceHandle::run( ) {
				get( ).run( );
			}


			void start_service( StartServiceMode mode ) {
				switch( mode ) {
				case StartServiceMode::Single:
					ServiceHandle::run( );
					break;
				case StartServiceMode::OnePerCore:
					for( int n = 0; n < static_cast<int>( std::thread::hardware_concurrency( ) ) -1; ++n ) {
						std::async( []( ) {
							ServiceHandle::run( );
						} );
					}
					ServiceHandle::run( );
					break;
				default:
					throw std::runtime_error( "Unknown StartServiceMode" );
				}
			}

// 			base::WorkQueue ServiceHandle::work_queue( ) {
// 				static base::WorkQueue queue( base::create_work_queue( ) );
// 				return queue;
// 			}
// 
// 			void ServiceHandle::add_work_item( std::function<void( )> work_item, std::function<void( base::OptionalError )> on_completion, bool auto_start ) { 
// 				work_queue( )->add_work_item( std::move( work_item ), std::move( on_completion ), std::move( auto_start ) );
// 			}


		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
