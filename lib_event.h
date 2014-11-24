#pragma once

#include <atomic>
#include <functional>
#include <string>
#include <type_traits>
#include <vector>

#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			using namespace daw::algorithm;

				template<typename... CallbackArgs>
				class Callback {
				public:
					using callback_function_t = std::function < void( CallbackArgs&&... ) > ;
					using id_t = int64_t;
				private:
					id_t m_id;
					callback_function_t m_callback_function;
					static std::atomic_uint_least64_t s_last_id;
				public:
					Callback( ) : m_id{ -1 }, m_callback_function{ } { }
					~Callback( ) = default;
					Callback( callback_function_t callback_function ): m_id( s_last_id++ ), m_callback_function( callback_function ) { }
				
					Callback( Callback const & ) = default;
				
					Callback& operator=(Callback const &) = default;

					Callback( Callback && other ) : m_id{ std::move( other.m_id ) }, m_callback_function{ std::move( other.m_callback_function ) } { }
				
					Callback& operator=( Callback && rhs ) {
						if( this != &rhs ) {
							m_id = std::move( rhs.m_id );
							m_callback_function = std::move( rhs.m_callback_function );						
						}
						return *this;
					}
					const id_t& id( ) const {
						return m_id;
					}

					bool empty( ) const {
						return m_id <= 0;
					}

					const callback_function_t& func( ) const;

					bool operator==(Callback const & rhs) const {
						return id( ) == rhs.id( );
					}

					bool operator<(Callback const &) = delete;
				};

				template<typename... CallbackArgs>
				auto make_callback( std::function<void( CallbackArgs&&... )> func ) -> decltype(Callback<CallbackArgs>( decltype( func ) )) {
					return Callback<CallbackArgs>( func );
				}

			template<typename... CallbackArgs>
			class EventEmitter {
			public:
				using callback_t = Callback<CallbackArgs...> ;
				using callback_function_t = std::function < void( CallbackArgs... ) > ;
			private:
				using callbacks_t = std::vector < std::pair<bool, callback_t> > ;
				using change_callback_t = std::function < void( EventEmitter const& ) > ;

				callbacks_t m_callbacks;
				change_callback_t m_on_new;
				change_callback_t m_on_remove;
				size_t m_max_listeners;

				bool exists( const callback_t& callback ) {
					return find_if( m_callbacks, [&callback]( std::pair<bool, callback_t> const & value ) {
						return callback == value.second;
					} ) != std::end( m_callbacks );
				}
			public:
				EventEmitter( ) : m_callbacks{ }, m_on_new{ }, m_on_remove{ }, m_max_listeners{ 0 } { }
				
				EventEmitter& on( callback_t const & callback ) {
					if( !exists( callback ) ) {
						m_callbacks.emplace_back( false, callback );
					}
					return *this;
				}	

				EventEmitter& set_max_listeners( size_t value ) {
					throw std::runtime_error( "Method not implemented" );
					/*return *this;*/
				}

				int64_t on( callback_function_t func ) {
					auto callback = callback_t( func );
					auto id = callback.id( );
					m_callbacks.emplace_back( false, callback );
					return id;
				}
				
				EventEmitter& once( callback_t const & callback ) {
					if( !exists( callback ) ) {
						m_callbacks.push_back( { true, callback } );
					}
					return *this;
				}

				auto once( callback_function_t func ) -> typename callback_t::id_t {
					auto callback = callback_t{ func };
					auto id = callback.id( );
					m_callbacks.emplace_back( false, callback );
					return id;
				}

				EventEmitter& remove_listener( callback_t const & callback ) {					
					erase_remove_if( m_callbacks, [&callback]( callback_t const & val ) {
						return callback.id( ) == val.id( );
					} );
					return *this;

				}

				EventEmitter& remove_all_listeners( ) { 
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

				virtual ~EventEmitter( ) { }
			};	// class IEvent

		} // namespace lib
	} // namespace nodepp
} // namespace daw


//////////////////////////////////////////////////////////////////////////
// Summary: Defines the following boilerplate inline 
// event_t_NAME = daw::nodepp::lib::EventEmitter&lt;__VA_ARGS__&gt;;
// event_t_NAME NAME;
// using NAME_callback_t = event_t_NAME::callback_t
#define CREATE_EVENT(NAME, ...) using event_t_ ## NAME = daw::nodepp::lib::EventEmitter<__VA_ARGS__>; event_t_## NAME NAME; using callback_t_ ## NAME = event_t_ ## NAME::callback_function_t
