#pragma once

#include <atomic>
#include <mutex>
#include <condition_variable>

namespace daw {
	namespace thread {

		using namespace std;

		//////////////////////////////////////////////////////////////////////////
		// Allows threads to wait for the counter to return to 0 after being 
		// incremented with reserve and decremented with notify
		template<typename Counter>
		class Semaphore : public std::enable_shared_from_this<Semaphore<Counter>> {
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

			std::shared_ptr<Semaphore<Counter>> get_ptr( ) {
				return shared_from_this( );
			}

			std::weak_ptr<Semaphore<Counter>> get_weak_ptr( ) {
				return get_ptr( );
			}

			bool dec_counter( ) {
				std::unique_lock<mutex> lck( m_mutex );
				auto result = --m_counter <= 0;
				lck.unlock( );
				m_condition.notify_one( );
				return result;
			}

			bool has_outstanding( ) {
				std::unique_lock<mutex> lck( m_mutex );
				auto result = m_counter >= 0;
				lck.unlock( );
				m_condition.notify_one( );
				return result;
			}

			void inc_counter( ) {
				std::unique_lock<mutex> lck( m_mutex );
				++m_counter;
			}

			void notify( ) {
				std::unique_lock<mutex> lck( m_mutex );
				m_condition.notify_all( );
			}
			
			void wait( ) {
				std::unique_lock<mutex> lck( m_mutex );
				m_condition.wait( lck, [&m_counter]( ) { return m_counter == 0; } );
				m_condition.notify_all( );
			}

			bool wait( size_t timeout_ms ) {
				std::unique_lock<mutex> lck( m_mutex );
				auto result = m_condition.wait_for( lck, std::chrono::milliseconds( timeout_ms ), [&]( ) { return m_counter == 0; } );
				m_condition.notify_all( );
				return result;
			}

		};	// class Semaphore

	}	// namespace thread
}	// namespace daw