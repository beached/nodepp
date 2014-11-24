#pragma once

#include <atomic>
#include <boost/any.hpp>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace base {
			namespace impl {
				template<typename T>
				T const & make_const( T const & val ) {
					return val;
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

					template<typename... Args>
					Callback( std::function<void( Args... )> stdfunction ) : m_id( s_last_id++ ), m_callback( stdfunction ), m_callback_type{ callback_type::stdfunction } { }

					Callback( Callback const & ) = default;

					Callback& operator=(Callback const &) = default;

					Callback( Callback && other );

					Callback& operator=(Callback && rhs);

					const id_t& id( ) const;

					bool empty( ) const;


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

			}	// namespace impl


			// Enum must have removeListener and newListener
			namespace generic {
				template<typename StringType = std::string>
				class EventEmitter {
					std::unordered_map<StringType, std::vector<std::pair<bool, impl::Callback>>> m_listeners;
					size_t m_max_listeners;
					std::string m_remove_listener_event;
					std::string m_new_listener_event;

					bool at_max_listeners( StringType event ) {
						return 0 != m_max_listeners && m_listeners[event].size( ) >= m_max_listeners;
					}

				public:
					using callback_id_t = typename impl::Callback::id_t;

					EventEmitter( StringType remove_listener_event = "removeListener", StringType new_listener_event = "newListener" ) :m_listeners{ }, m_max_listeners{ 10 }, m_remove_listener_event( remove_listener_event ), m_new_listener_event( new_listener_event ) { }

					std::string const & remove_event_listener_name( ) const {
						return m_remove_listener_event;
					}

					std::string const & new_event_listener_name( ) const {
						return m_new_listener_event;
					}

					template<typename Listener>
					callback_id_t add_listener( StringType event, Listener& listener, bool run_once = false ) {
						if( !at_max_listeners( event ) ) {
							auto callback = impl::Callback{ listener };
							m_listeners[event].emplace_back( run_once, callback );
							emit( m_new_listener_event, event, callback );
							return callback.id( );
						} else {
							// TODO: implement logging to fail gracefully.  For now throw
							throw std::runtime_error( "Max listeners reached for event" );
						}
					}

					template<typename Listener>
					EventEmitter& on( StringType event, Listener& listener ) {
						add_listener( event, listener );
						return *this;
					}

					template<typename Listener>
					EventEmitter& once( StringType event, Listener& listener ) {
						add_listener( event, true );
						return *this;
					}

					EventEmitter& remove_listener( StringType event, callback_id_t id ) {
						daw::algorithm::erase_remove_if( m_listeners[event], [&id]( Callback const & item ) {
							emit( m_remove_listener_event, event, item );
							return item.id( ) == id;
						} );
						return *this;
					}

					EventEmitter& remove_listener( StringType event, impl::Callback listener ) {
						return remove_listener( event, listener.id( ) );
					}

					EventEmitter& remove_all_listeners( ) {
						m_listeners.clear( );
						return *this;
					}

					EventEmitter& remove_all_listeners( StringType event ) {
						if( )
							m_listeners[event].clear( );
						return *this;
					}

					EventEmitter& set_max_listeners( size_t max_listeners ) {
						m_max_listeners = m_max_listeners;
					}

					auto listeners( StringType event ) -> decltype(impl::make_const( m_listeners[event] )) {
						return m_listeners[event];
					}

					template<typename... Args>
					EventEmitter& emit( StringType event, Args&&... args ) {
						for( auto& callback : m_listeners[event] ) {
							callback.second.exec( std::forward<Args>( args )... );
						}
						daw::algorithm::erase_remove_if( m_listeners[event], []( std::pair<bool, impl::Callback> const & item ) {
							return item.first;
						} );
						return *this;
					}

					static size_t listener_count( EventEmitter const & emitter, StringType event ) {
						return emitter.listeners( event ).size( );
					}

				};	// class EventEmitter
			}	// namespace generic
			using EventEmitter = generic::EventEmitter < > ;
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
