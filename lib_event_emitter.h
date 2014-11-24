#pragma once

#include <atomic>
#include <boost/any.hpp>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "range_algorithm.h"
#include "utility.h"

namespace daw {
	namespace nodepp {
		namespace base {
			namespace impl {
				template<typename T>
				T const & make_const( T const & val ) {
					return val;
				}
			}

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
						auto callback = (daw::function_pointer_t<void, Args...>)(boost::any_cast<void*>(m_callback));
						callback( std::forward<Args>( args )... );
					}
												 break;
					case callback_type::stdfunction: {
						using callback_t = std::function < void( Args... ) > ;
						auto& callback = boost::any_cast<callback_t>(m_callback);
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


			// Enum must have removeListener and newListener
			namespace generic {
				template<typename EventKeyType = std::string>
				class EventEmitter {
					std::unordered_map<EventKeyType, std::vector<std::pair<bool, Callback>>> m_listeners;
					size_t m_max_listeners;
					EventKeyType m_remove_listener_event;
					EventKeyType m_new_listener_event;

					bool at_max_listeners( EventKeyType event ) {
						return 0 != m_max_listeners && m_listeners[event].size( ) >= m_max_listeners;
					}

				public:
					using callback_id_t = typename Callback::id_t;

					EventEmitter( EventKeyType remove_listener_event, EventKeyType new_listener_event ) :m_listeners{ }, m_max_listeners{ 10 }, m_remove_listener_event( remove_listener_event ), m_new_listener_event( new_listener_event ) { }

					virtual ~EventEmitter( ) { }					

					template<typename Listener>
					callback_id_t add_listener( EventKeyType event, Listener& listener, bool run_once = false ) {
						if( !at_max_listeners( event ) ) {
							auto callback = Callback{ listener };
							m_listeners[event].emplace_back( run_once, callback );
							emit( m_new_listener_event, event, callback );
							return callback.id( );
						} else {
							// TODO: implement logging to fail gracefully.  For now throw
							throw std::runtime_error( "Max listeners reached for event" );
						}
					}

					template<typename Listener>
					EventEmitter& on( EventKeyType event, Listener& listener ) {
						add_listener( event, listener );
						return *this;
					}

					template<typename Listener>
					EventEmitter& once( EventKeyType event, Listener& listener ) {
						add_listener( event, true );
						return *this;
					}

					virtual EventEmitter& remove_listener( EventKeyType event, callback_id_t id ) {
						daw::algorithm::erase_remove_if( m_listeners[event], [&]( std::pair<bool, Callback> const & item ) {
							if( item.second.id( ) == id ) {
								emit( m_remove_listener_event, event, item );
								return true;
							}
							return false;
						} );
						return *this;
					}

					virtual EventEmitter& remove_listener( EventKeyType event, Callback listener ) {
						return remove_listener( event, listener.id( ) );
					}

					virtual EventEmitter& remove_all_listeners( ) {
						m_listeners.clear( );
						return *this;
					}

					virtual EventEmitter& remove_all_listeners( EventKeyType event ) {
						m_listeners[event].clear( );
						return *this;
					}

					virtual EventEmitter& set_max_listeners( size_t max_listeners ) {
						m_max_listeners = m_max_listeners;
						return *this;
					}

					virtual auto listeners( EventKeyType event ) -> decltype(impl::make_const( m_listeners[event] )) {
						return m_listeners[event];
					}

					template<typename... Args>
					EventEmitter& emit( EventKeyType event, Args&&... args ) {
						for( auto& callback : m_listeners[event] ) {
							callback.second.exec( std::forward<Args>( args )... );
						}
						daw::algorithm::erase_remove_if( m_listeners[event], []( std::pair<bool, Callback> const & item ) {
							return item.first;
						} );
						return *this;
					}

					static size_t listener_count( EventEmitter const & emitter, EventKeyType event ) {
						return emitter.listeners( event ).size( );
					}

					template<typename Listener, typename Action>
					auto rollback_event_on_exception( EventKeyType event, Listener listener, Action action_to_try, bool run_listener_once = false ) -> decltype(action_to_try( )) {
						auto cb_id = add_listener( event, listener, run_listener_once );
						try {
							return action_to_try( );
						} catch( ... ) {
							// Rollback listener
							remove_listener( event, cb_id );
							std::rethrow_exception( std::current_exception( ) );
						}
					}

				};	// class EventEmitter
			}	// namespace generic
			using EventEmitter = generic::EventEmitter < > ;
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
