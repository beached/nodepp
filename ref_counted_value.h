#pragma once

#include <functional>
#include <cstdint>

namespace daw {

	template<typename T>
	class ReferenceCountedValue {
		using counter_t = int64_t;
		T* m_value;
		counter_t* m_counter;
		std::function<void( T* )> m_cleaner;
	public:
		
		template<typename... Args>
		ReferenceCountedValue( Args&&... args ) : m_value{ new(std::nothrow) T( std::forward<Args&&>( args )... ) }, m_counter{ new(std::nothrow) counter_t( ) }, m_cleaner{ nullptr } {
			*m_counter = 1;
		}

		explicit ReferenceCountedValue( T&& value ) : m_value{ new(std::nothrow) T( std::forward<T>( value ) ) }, m_counter{ new(std::nothrow) counter_t( ) }, m_cleaner{ nullptr } {
			*m_counter = 1;
		}

		explicit ReferenceCountedValue( const ReferenceCountedValue& other ) : m_value{ other.m_value }, m_counter{ other.m_counter }, m_cleaner{ other.m_cleaner } {
			++(*other.m_counter);
		}

		ReferenceCountedValue& operator=(const ReferenceCountedValue& rhs) {
			if( this != &rhs ) {
				m_value = rhs.m_value;
				m_counter = rhs.m_counter;
				++(*rhs.m_counter);
				m_cleaner = rhs.m_cleaner;
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

	};


}