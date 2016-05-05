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
#include <boost/any.hpp>
#include <functional>
#include <stdexcept>

#include <daw/daw_utility.h>

namespace daw {
	namespace nodepp {
		namespace base {
			//////////////////////////////////////////////////////////////////////////
			// Summary:		CallbackImpl wraps a std::function or a c-style function ptr.
			//				This is needed because std::function are not comparable
			//				to each other.
			// Requires:
			class Callback final {
			public:
				using id_t = int64_t;
			private:
				static std::atomic_int_least64_t s_last_id;

				id_t m_id;
				boost::any m_callback;
			public:
				Callback( );
				~Callback( ) = default;

				template<typename Listener, typename = typename std::enable_if_t<!std::is_same<Listener, Callback>::value>>
				Callback( Listener listener ): m_id( s_last_id++ ), m_callback( daw::make_function( listener ) ) { }

				Callback( Callback const & ) = default;

				Callback& operator=( Callback const & ) = default;

				Callback( Callback && ) = default;

				Callback& operator=( Callback && ) = default;

				const id_t& id( ) const;

				void swap( Callback& rhs );

				bool operator==( Callback const & rhs ) const;

				bool empty( ) const;

				template<typename... Args>
				void operator( )( Args&&... args ) {
					using cb_type = std::function <void( typename std::remove_reference_t<Args>... )>;
					try {
						auto callback = boost::any_cast<cb_type>(m_callback);
						callback( std::forward<Args>( args )... );
					} catch( boost::bad_any_cast const & ) {
						throw std::runtime_error( "Type of event listener does not match.  This shouldn't happen" );
					}
				}
			};
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw

