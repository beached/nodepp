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
			// Summary:		Callback wraps a std::function or a c-style function ptr.
			//				This is needed because std::function are not comparable
			//				to each other.
			// Requires:
			class Callback {
			public:
				using id_t = int64_t;
			private:
				enum class callback_type { none = 0, stdfunction, funcptr };
				static std::atomic_uint_least64_t s_last_id;

				id_t m_id;
				boost::any m_callback;
				callback_type m_callback_type;
			public:
				Callback( );
				~Callback( ) = default;

				Callback( void* funcptr );
				Callback& operator=(void* funcptr);

				template<typename... Args>
				Callback( std::function<void( Args... )> stdfunction ) : m_id( s_last_id++ ), m_callback( stdfunction ), m_callback_type{ callback_type::stdfunction } { }

				template<typename... Args>
				Callback& operator=(std::function<void( Args... )> stdfunction) {
					auto tmp = Callback( stdfunction );
					tmp.swap( *this );
					return *this;
				}

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
					switch( m_callback_type ) {
					case callback_type::funcptr: {
						//auto callback{ (daw::function_pointer_t<void, Args...>)(boost::any_cast<void*>(m_callback)) };
						auto callback{ static_cast<daw::function_pointer_t<void, Args...>>(boost::any_cast<void*>(m_callback)) };

						callback( std::forward<Args>( args )... );
					}
					break;
					case callback_type::stdfunction: {
						using callback_t = std::function < void( Args... ) > ;
						auto callback{ boost::any_cast<callback_t>(m_callback) };
						callback( std::forward<Args>( args )... );
					}
					break;
					case callback_type::none:
						throw std::runtime_error( "Attempt to execute a empty callback" );
					default:
						throw std::runtime_error( "Unexpected callback type" );
					}
				}

			};
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
