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

				id_t m_id;
				boost::any m_callback;
			public:
				Callback( );
				~Callback( ) = default;

				template<typename Listener, typename = typename std::enable_if<!std::is_same<Listener, Callback>::value>::type>
				Callback( Listener listener ) : m_id( s_last_id++ ), m_callback( daw::make_function( listener ) ) { }

				Callback( Callback const & ) = default;

				Callback& operator=(Callback const &) = default;

				Callback( Callback && other );

				Callback& operator=(Callback && rhs);

				const id_t& id( ) const;

				void swap( Callback& rhs );

				bool operator==(Callback const & rhs) const;

				bool empty( ) const;

				template<typename... Args>
				void exec( Args&&... args ) {					
					//using cb_type = remove_const<std::function < void( typename std::remove_reference<Args>::type... ) >>::type;
					using cb_type = std::function < void( typename std::remove_reference<Args>::type... ) >;
					auto callback = boost::any_cast<cb_type>(m_callback);
					callback( std::forward<Args>( args )... );
				}

			};

		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
