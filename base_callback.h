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
			//				to each other.  Must explicitly convert lambda's to
			//				std::functions
			// Requires:
			class Callback {
			public:
				using id_t = int64_t;
			private:
				enum class Callbackype { none = 0, stdfunction, funcptr };
				static std::atomic_int_least64_t s_last_id;
				using cb_storage_t = void*;
				id_t m_id;
				boost::any m_callback;
				Callbackype m_Callbackype;

			public:
				Callback( );
				~Callback( ) = default;

				Callback( void* listener );

				template<typename... Args>
				Callback( std::function<void( Args... )> listener ) : m_id( s_last_id++ ), m_callback( listener ), m_Callbackype( Callbackype::stdfunction ) { }

				Callback( Callback const & ) = default;

				Callback& operator=(Callback const &) = default;

				Callback( Callback && other );

				Callback& operator=(Callback && rhs);

				const id_t& id( ) const;

				bool empty( ) const;

				void swap( Callback& rhs );

				bool operator==(Callback const & rhs) const;

				template<typename... Args>
				void exec( Args&&... args ) {
					switch( m_Callbackype ) {
					case Callbackype::funcptr: {
						auto callback = reinterpret_cast<daw::function_pointer_t<void, Args...>>(boost::any_cast<void*>(m_callback));
						callback( std::forward<Args>( args )... );
					}
												 break;
					case Callbackype::stdfunction: {
						auto callback = boost::any_cast<std::function < void( Args... ) >>(m_callback);
						callback( std::forward<Args>( args )... );
					}
													 break;
					case Callbackype::none:
						throw std::runtime_error( "Attempt to execute a empty callback" );
					default:
						throw std::runtime_error( "Unexpected callback type" );
					}
				}

			};

		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
