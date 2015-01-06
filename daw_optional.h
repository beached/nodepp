

#include <functional>

#include "make_unique.h"

namespace daw {	
	class OptionalNotSetException: public std::exception { };

	template<typename T>
	class Optional {
		std::unique_ptr<T> m_value;
	public:
		using value_t = T;
		//////////////////////////////////////////////////////////////////////////
		/// Summary: no value
		Optional( ) : m_value( nullptr ) { }

		//////////////////////////////////////////////////////////////////////////
		/// Summary: Initialize with a value
		explicit Optional( T value ) : m_value( make_unique<T>( std::move( value ) ) ) { }

		Optional( Optional const & other ) : m_value( make_unique( ) ) { 
			*this = other;
		}
		
		Optional& operator=(Optional const & rhs ) {
			if( this != &rhs && rhs.m_value ) {
				*m_value = *rhs.m_value;
			}
			return *this;
		}

		Optional( Optional && other ) : m_value( std::move( other.m_value ) ) { }

		Optional& operator=(Optional && rhs) {
			if( this != &rhs ) {
				m_value = std::move( rhs.m_value );
			}
			return *this;
		}

		Optional( std::function<T( )> func ): m_value( ) {			
			try {
				m_value = make_unique<T>( func( ) );
			} catch( ... ) {
				return;
			}			
		}

		Optional& and_then( std::function <T( T const & )> func ) {
			if( !empty( ) ) {
				*this = Optional<T>( func( get( ) ) );
			}
			return *this;
		}

		T const & operator*() const {
			if( !m_value ) {
				throw OptionalNotSetException( );
			}
			return *m_value;
		}

		T & operator*() {
			if( !m_value ) {
				throw OptionalNotSetException( );
			}
			return *m_value;
		}

		template<typename... Args>
		Optional& emplace( Args&&... args ) {
			m_value.reset( make_unique<Args>( std::forward<Args>( args )... ) );
		}

		bool empty( ) const {
			return !m_value;
		}

		explicit operator bool( ) const {
			return !empty( );
		}

		T const & get( ) const {
			if( !m_value ) {
				throw OptionalNotSetException( );
			}
			return *m_value;
		}

		T & get( ) {
			if( !m_value ) {
				throw OptionalNotSetException( );
			}
			return *m_value;
		}

		T const & get_value_or( T const& default_value ) const {
			if( m_value ) {
				return *m_value;
			}
			return default_value;
		}

		bool operator==(Optional const & rhs) {
			if( m_value && rhs.m_value ) {
				return *m_value == *rhs.m_value;
			}
			return !m_value && !rhs.m_value;
		}

		bool operator!=(Optional const & rhs) {
			return !(*this == rhs);
		}

		bool operator<(Optional const & rhs) {
			return *m_value < *rhs.m_value;	
		}

		bool operator>(Optional const & rhs) {
			return *m_value > *rhs.m_value;
		}

		bool operator<=(Optional const & rhs) {
			return *m_value <= *rhs.m_value;
		}

		bool operator>=( Optional const & rhs ) {
			return *m_value >= *rhs.m_value;
		}

		bool operator==(T const & rhs) {
			return *m_value == rhs;
		}

		bool operator!=(T const & rhs) {
			return *m_value != rhs;
		}

		bool operator<(T const & rhs) {
			return *m_value < rhs;
		}

		bool operator>( T const & rhs ) {
			return *m_value > rhs;
		}

		bool operator<=(T const & rhs) {
			return *m_value <= rhs;
		}

		bool operator>=(T const & rhs) {
			return *m_value >= rhs;
		}

	};	// class Optional



}	// namespace daw
