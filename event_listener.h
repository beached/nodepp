#pragma once

#include <atomic>
#include <functional>
#include <string>
#include <vector>

#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace base {
			using namespace daw::algorithm;

			template<typename... CallbackArgs>
			class Callback {
			public:
				using callback_function_t = std::function < void( CallbackArgs&&... ) > ;
				using id_t = uint64_t;
			private:
				id_t m_id;
				callback_function_t m_callback_function;
				static std::atomic_uint_least64_t s_last_id;
			public:
				Callback( callback_function_t callback_function ): m_id( s_last_id++ ), m_callback_function( callback_function ) { }
				const id_t& id( ) const {
					return m_id;
				}
				const callback_function_t& func( ) const;

				bool operator==(Callback const & rhs) const {
					return id( ) == rhs.id( );
				}
			};

			template<typename... CallbackArgs>
			class Event {
			public:
				using callback_t = Callback<CallbackArgs...> ;
			private:
				using callbacks_t = std::vector < std::pair<bool, callback_t> > ;
				using change_callback_t = std::function < void( Event const& ) > ;

				callbacks_t m_callbacks;
				change_callback_t m_on_new;
				change_callback_t m_on_remove;

				bool exists( const callback_t& callback ) {
					return find( m_callbacks, callback, []( std::pair<bool, callback_t> const & lhs, std::pair<bool, callback_t> const & rhs ) {
						return lhs.second == rhs.second;
					} ) != std::end( m_callbacks );
				}
			public:
				Event( ): m_callbacks( ), m_on_new( ), m_on_remove( ) { }

				Event& on( callback_t const & callback ) {
					if( !exists( callback ) ) {
						m_callbacks.push_back( { false, callback } )
					}
					return *this;
				}
				
				Event& once( callback_t const & call_back ) {
					if( !exists( callback ) ) {
						m_callbacks.push_back( { true, callback } )
					}
					return *this;
				}

				Event& remove_listener( callback_t const & call_back ) {					
					erase_remove_if( m_callbacks, [&call_back]( callback_t const & val ) {
						return call_back.id( ) == val.id( );
					} );
					return *this;

				}

				Event& remove_all_listeners( ) { 
					m_callbacks.clear;
					return *this;
				}

				callbacks_t& listeners( ) {
					return m_callbacks;
				}

				callbacks_t const & listeners( ) const {
					return m_callbacks;
				}

				bool emit( CallbackArgs&&... args ) { 
					for( auto const & callback : m_callbacks ) {
						callback( std::forward<CallbackArgs>( args )... );
					}
				}

				size_t listener_count( ) const { 
					return m_callbacks.size( );
				}

				change_callback_t const & new_listener( ) const { 
					return m_on_new;
				}

				change_callback_t & new_listener( ) {
					return m_on_new;
				}

				change_callback_t const & remove_listener( ) const {
					return m_on_remove;
				}

				change_callback_t & remove_listener( ) {
					return m_on_remove;
				}

				virtual ~Event( ) { }
			};	// class IEvent

		} // namespace base
	} // namespace nodepp
} // namespace daw
