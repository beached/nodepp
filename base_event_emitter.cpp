#include <boost/asio/error.hpp>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "base_event_emitter.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace base {	
			EventEmitter::EventEmitter( ) :
				m_listeners( std::make_shared<listeners_t>( ) ),
				m_max_listeners( 10 ) { }

			EventEmitter::EventEmitter( EventEmitter && other ):
				m_listeners( std::move( other.m_listeners ) ),
				m_max_listeners( std::move( other.m_max_listeners ) ) {
				std::cerr << "EventEmitter::EventEmitter( EventEmmiter && )\n";
			}

			EventEmitter& EventEmitter::operator=(EventEmitter && rhs) {
				if( this != &rhs ) {
					m_listeners = std::move( rhs.m_listeners );
					m_max_listeners = std::move( rhs.m_max_listeners );
				}
				std::cerr << "EventEmitter::operator=( EventEmitter&& )\n";
				return *this;
			}

			void EventEmitter::swap( EventEmitter& rhs ) {
				std::cerr << "EventEmitter::swap( EventEmitter & )\n";
				using std::swap;
				swap( m_listeners, rhs.m_listeners );
				swap( m_max_listeners, rhs.m_max_listeners );
			}

			std::unordered_map<std::string, EventEmitter::listener_list_t> & EventEmitter::listeners( ) {
				return *m_listeners;
			}

			bool EventEmitter::at_max_listeners( std::string event ) {
				auto result = 0 != m_max_listeners;
				result &= listeners( )[event].size( ) >= m_max_listeners;
				return result;				
			}
			
			void EventEmitter::remove_listener( std::string event, callback_id_t id ) {
				daw::algorithm::erase_remove_if( listeners( )[event], [&]( std::pair<bool, Callback> const & item ) {
					if( item.second.id( ) == id ) {
						// TODO verify if this needs to be outside loop
						emit_listener_removed( event, item.second );
						return true;
					}
					return false;
				} );
			}			

			void EventEmitter::remove_listener( std::string event, Callback listener ) {
				return remove_listener( event, listener.id( ) );
			}

			void EventEmitter::remove_all_listeners( ) {
				listeners( ).clear( );
			}

			void EventEmitter::remove_all_listeners( std::string event ) {
				listeners( )[event].clear( );
			}

			void EventEmitter::set_max_listeners( size_t max_listeners ) {
				m_max_listeners = max_listeners;
			}

			EventEmitter::listener_list_t EventEmitter::listeners( std::string event ) {
				return listeners( )[event];
			}

			size_t EventEmitter::listener_count( std::string event ) {
				return listeners( event ).size( );
			}

			std::shared_ptr<EventEmitter> EventEmitter::get_ptr( ) {
				return shared_from_this( );
			}

			void EventEmitter::emit_listener_added( std::string event, Callback listener ) {
				emit( "listener_added", std::move( event ), std::move( listener ) );
			}

			void EventEmitter::emit_listener_removed( std::string event, Callback listener ) {
				emit( "listener_removed", std::move( event ), std::move( listener ) );
			}


		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
