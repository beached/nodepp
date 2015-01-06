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

			Callback::Callback( ) : m_id{ -1 }, m_callback{ 0 } { }

			Callback::Callback( Callback && other ) : m_id{ std::move( other.m_id ) }, m_callback{ std::move( other.m_callback ) } { }

			Callback& Callback::operator=(Callback && rhs) {
				if( this != &rhs ) {
					m_id = std::move( rhs.m_id );
					m_callback = std::move( rhs.m_callback );
				}
				return *this;
			}

			const Callback::id_t& Callback::id( ) const {
				return m_id;
			}

			bool Callback::operator==(Callback const & rhs) const {
				return id( ) == rhs.id( );
			}

			bool Callback::empty( ) const {
				return -1 == m_id;
			}

			void Callback::swap( Callback& rhs ) {
				using std::swap;
				swap( m_id, rhs.m_id );
				swap( m_callback, rhs.m_callback );
			}
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw

