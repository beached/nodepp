#include <string>
#include <utility>
#include <vector>

#include "base_event_emitter.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace base {
			bool EventEmitter::at_max_listeners( std::string event ) {
				return 0 != m_max_listeners && m_listeners[event].size( ) >= m_max_listeners;
			}

			char const * const EventEmitter::c_new_listener_event = "newListener";
			char const * const EventEmitter::c_remove_listener_event = "removeListener";

			EventEmitter::EventEmitter( ) :m_listeners{ }, m_max_listeners{ 10 } { }

			EventEmitter::~EventEmitter( ) { }

			bool EventEmitter::event_is_valid( std::string const & event ) const {
				static std::vector<std::string> const valid_events = { c_new_listener_event, c_remove_listener_event };
				return daw::algorithm::find( valid_events, event ) != valid_events.end( );
			}

			EventEmitter& EventEmitter::remove_listener( std::string event, callback_id_t id ) {
				daw::algorithm::erase_remove_if( m_listeners[event], [&]( std::pair<bool, Callback> const & item ) {
					if( item.second.id( ) == id ) {
						emit( c_remove_listener_event, event, item );
						return true;
					}
					return false;
				} );
				return *this;
			}

			EventEmitter& EventEmitter::remove_listener( std::string event, Callback listener ) {
				return remove_listener( event, listener.id( ) );
			}

			EventEmitter& EventEmitter::remove_all_listeners( ) {
				m_listeners.clear( );
				return *this;
			}

			EventEmitter& EventEmitter::remove_all_listeners( std::string event ) {
				m_listeners[event].clear( );
				return *this;
			}

			EventEmitter& EventEmitter::set_max_listeners( size_t max_listeners ) {
				m_max_listeners = m_max_listeners;
				return *this;
			}

			EventEmitter::listener_list_t EventEmitter::listeners( std::string event ) {
				return m_listeners[event];
			}


			size_t EventEmitter::listener_count( EventEmitter& emitter, std::string event ) {
				return emitter.listeners( event ).size( );
			}


		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
