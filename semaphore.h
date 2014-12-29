#pragma once

#include <atomic>
#include <mutex>
#include <condition_variable>

#include "base_event_emitter.h"


namespace daw {
	namespace thread {

		//////////////////////////////////////////////////////////////////////////
		// Allows threads to wait for the counter to return to 0 after being 
		// incremented with reserve and decremented with notify
		template<typename Counter>
		class Semaphore : public daw::nodepp::base::enable_shared<Semaphore<Counter> > {
			std::mutex m_mutex;
			std::condition_variable m_condition;
			Counter m_counter;

		public:
			Semaphore( Counter count = 0 ) :m_counter( count ) { }
			Semaphore( Semaphore const & ) = delete;
			Semaphore( Semaphore && ) = delete;
			Semaphore& operator=(Semaphore const &) = delete;
			Semaphore& operator=(Semaphore&&) = delete;
			~Semaphore( ) = default;

			bool dec_counter( ) {
				std::unique_lock<std::mutex> lck( m_mutex );
				auto result = --m_counter <= 0;
				lck.unlock( );
				m_condition.notify_one( );
				return result;
			}

			bool has_outstanding( ) {
				std::unique_lock<std::mutex> lck( m_mutex );
				auto result = m_counter >= 0;
				lck.unlock( );
				m_condition.notify_one( );
				return result;
			}

			void inc_counter( ) {
				std::unique_lock<std::mutex> lck( m_mutex );
				++m_counter;
			}

			void notify( ) {
				std::unique_lock<std::mutex> lck( m_mutex );
				m_condition.notify_all( );
			}

			Counter const & size( ) const {
				std::unique_lock<std::mutex> lck( m_mutex );
				return m_counter;
			}
			
			void lock_and_do( std::function<void( Counter const & )> action ) {
				std::unique_lock<std::mutex> lck( m_mutex );
				action( m_counter );
			}

			void wait( ) {
				std::unique_lock<std::mutex> lck( m_mutex );
				m_condition.wait( lck, [&]( ) { return m_counter == 0; } );
				m_condition.notify_all( );
			}

			bool wait( size_t timeout_ms ) {
				std::unique_lock<std::mutex> lck( m_mutex );
				auto result = m_condition.wait_for( lck, std::chrono::milliseconds( timeout_ms ), [&]( ) { return m_counter == 0; } );
				m_condition.notify_all( );
				return result;
			}

		};	// class Semaphore

	}	// namespace thread
}	// namespace daw
