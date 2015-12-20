// The MIT License (MIT)
// 
// Copyright (c) 2014-2015 Darrell Wright
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

#include <functional>
#include <cstdint>
#include "../header_libraries/daw_exception.h"
namespace daw {

	template<typename T>
	class ReferenceCountedValue final {
		using counter_t = int64_t;
		T* m_value;
		counter_t* m_counter;
		std::function<void( T* )> m_cleaner;
	public:
		
		template<typename... Args>
		ReferenceCountedValue( Args&&... args ) : m_value{ new T( std::forward<Args&&>( args )... ) }, m_counter{ new counter_t( ) }, m_cleaner{ nullptr } {
			daw::exception::daw_throw_on_null( m_counter, "Unable to allocate value" );
			*m_counter = 1;
		}

		explicit ReferenceCountedValue( T&& value ) : m_value{ new T( std::forward<T>( value ) ) }, m_counter{ new counter_t( ) }, m_cleaner{ nullptr } {			
			daw::exception::daw_throw_on_null( m_counter, "Unable to allocate value" );
			*m_counter = 1;
		}

		ReferenceCountedValue( ReferenceCountedValue const& other ) : m_value{ other.m_value }, m_counter{ other.m_counter }, m_cleaner{ other.m_cleaner } {
			++(*other.m_counter);
		}

		ReferenceCountedValue& operator=(ReferenceCountedValue const& rhs) {
			if( this != &rhs ) {
				m_value = rhs.m_value;
				m_counter = rhs.m_counter;
				++(*rhs.m_counter);
				m_cleaner = rhs.m_cleaner;
			}
			return *this;
		}

		ReferenceCountedValue( ReferenceCountedValue && other ) : m_value{ std::move( other.m_value ) }, m_counter{ std::move( other.m_counter ) }, m_cleaner{ std::move( other.m_cleaner ) } {
			++(*other.m_counter);
		}

		ReferenceCountedValue& operator=( ReferenceCountedValue && rhs) {
			if( this != &rhs ) {
				m_value = std::move( rhs.m_value );
				m_counter = std::move( rhs.m_counter );
				++(*rhs.m_counter);
				m_cleaner = std::move( rhs.m_cleaner );
			}
			return *this;
		}


		void set_cleaner( std::function<void( T* )> cleaner ) {
			m_cleaner = cleaner;
		}

		void clear_cleaner( ) {
			m_cleaner = std::function<void( T* )>( nullptr );
		}

		T& operator()( ) {
			return *m_value;
		}

		const T& operator()( ) const { 
			return *m_value;
		}

		counter_t count( ) const {
			return *m_counter;
		}

		bool empty( ) const {
			return !m_value && *m_counter <= 1;
		}
 
		~ReferenceCountedValue( ) {
			if( m_value ) {
				if( *m_counter <= 1 ) {
					*m_counter = 0;
					if( m_cleaner ) {
						m_cleaner( m_value );
					}
					delete m_value;
					m_value = nullptr;
				} else {
					--(*m_counter);
				}
			}
		}

	};	// class ReferencedCountedValue
}	// namespace daw

