#include <string>
#include <utility>
#include <vector>

#include "base_event_emitter.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace base {			

			std::vector<std::string> const & EventEmitter::valid_events( ) const {
				static auto const result = std::vector < std::string > { "newListener", "removeListener" };
				return result;
			}

			bool EventEmitter::at_max_listeners( std::string event ) {
				return 0 != m_max_listeners && m_listeners[event].size( ) >= m_max_listeners;
			}

			EventEmitter::EventEmitter( ) :m_listeners{ }, m_max_listeners{ 10 } { }

			EventEmitter::~EventEmitter( ) { }
			
			EventEmitter& EventEmitter::remove_listener( std::string event, callback_id_t id ) {
				daw::algorithm::erase_remove_if( m_listeners[event], [&]( std::pair<bool, Callback> const & item ) {
					if( item.second.id( ) == id ) {
						emit( "removeListener", event, item );
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
				m_max_listeners = max_listeners;
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
