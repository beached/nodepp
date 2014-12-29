#pragma once

#include <boost/asio.hpp>
#include <functional>
#include <memory>

#include "base_error.h"
#include "base_work_queue.h"

namespace daw {
	namespace nodepp {
		namespace base {
			class ServiceHandle {
			public:
				static boost::asio::io_service& get( );
				static void run( );

				//////////////////////////////////////////////////////////////////////////
				/// Summary:	A thread pool that will perform tasks with at most 
				///				the number of cores on the machine
				static base::WorkQueue work_queue( );
				
				//////////////////////////////////////////////////////////////////////////
				/// Summary:	Adds a work item to the thread pool with optional
				///				callback on completion.
				//static void add_work_item( std::function<void( )> work_item, std::function<void( base::OptionalError )> on_completion = nullptr, bool auto_start = true );

			};	// class ServiceHandle

			enum class StartServiceMode { Single, OnePerCore };

			void start_service( StartServiceMode mode );
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
