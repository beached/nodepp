
#include <atomic>
#include <boost/any.hpp>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <utility>

#include "base_callback.h"

namespace daw {
	namespace nodepp {
		namespace base {
			std::atomic_int_least64_t Callback::s_last_id{ 1 };

			Callback::Callback( ) : m_id{ -1 }, m_callback{ 0 }, m_Callbackype{ Callbackype::none } { }

			Callback::Callback( void* funcptr ) : m_id( s_last_id++ ), m_callback( funcptr ), m_Callbackype{ Callbackype::funcptr } { }

			Callback::Callback( Callback && other ) : m_id{ std::move( other.m_id ) }, m_callback{ std::move( other.m_callback ) }, m_Callbackype{ std::move( other.m_Callbackype ) } { }

			Callback& Callback::operator=(Callback && rhs) {
				if( this != &rhs ) {
					m_id = std::move( rhs.m_id );
					m_callback = std::move( rhs.m_callback );
					m_Callbackype = std::move( rhs.m_Callbackype );
				}
				return *this;
			}

			const Callback::id_t& Callback::id( ) const {
				return m_id;
			}

			bool Callback::empty( ) const {
				return m_Callbackype == Callbackype::none;
			}

			bool Callback::operator==(Callback const & rhs) const {
				return id( ) == rhs.id( );
			}

			void Callback::swap( Callback& rhs ) {
				using std::swap;
				swap( m_id, rhs.m_id );
				swap( m_callback, rhs.m_callback );
				swap( m_Callbackype, rhs.m_Callbackype );
			}
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw

