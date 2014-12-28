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
					for( unsigned int n = 0; n < std::thread::hardware_concurrency( ); ++n ) {
						std::async( []( ) {
							ServiceHandle::run( );
						} );
					}
					break;
				default:
					throw std::runtime_error( "Unknown StartServiceMode" );
				}
			}
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
