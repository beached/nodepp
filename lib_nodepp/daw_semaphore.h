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
		class Semaphore final: public daw::nodepp::base::enable_shared <Semaphore<Counter>> {
			mutable std::mutex m_mutex;
			std::condition_variable m_condition;
			Counter m_counter;

		public:
			explicit Semaphore( Counter count = 0 ) :m_counter( count ) { }
			Semaphore( Semaphore const & ) = delete;
			Semaphore( Semaphore && ) = default;
			Semaphore& operator=(Semaphore const &) = delete;
			Semaphore& operator=(Semaphore&&) = default;
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
				auto result = m_counter>= 0;
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

			Counter const & count( ) const {
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
