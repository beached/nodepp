#pragma once

#include <atomic>
#include <boost/any.hpp>
#include <cstdint>
#include <functional>
#include <stdexcept>

#include "utility.h"

namespace daw {
	namespace nodepp {
		namespace base {
			//////////////////////////////////////////////////////////////////////////
			// Summary:		CallbackImpl wraps a std::function or a c-style function ptr.
			//				This is needed because std::function are not comparable
			//				to each other. 
			// Requires:
			class Callback {
			public:
				using id_t = int64_t;
			private:
				static std::atomic_int_least64_t s_last_id;
				//using cb_storage_t = void*;

				id_t m_id;
				boost::any m_callback;
			public:
				Callback( );
				~Callback( ) = default;

				//Callback( void* listener );

				template<typename Listener>
				Callback( Listener&& listener ) : m_id( s_last_id++ ), m_callback( daw::make_function( std::forward<Listener>( listener ) ) ) { }

				Callback( Callback const & ) = default;

				Callback& operator=(Callback const &) = default;

				Callback( Callback && other );

				Callback& operator=(Callback && rhs);

				const id_t& id( ) const;

				void swap( Callback& rhs );

				bool operator==(Callback const & rhs) const;

				template<typename... Args>
				void exec( Args&&... args ) {
					auto callback = boost::any_cast<std::function < void( Args... ) >>(m_callback);
					callback( std::forward<Args>( args )... );
				}

			};

		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
