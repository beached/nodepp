#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "base_event_emitter.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace base {	

			std::unordered_map<std::string, EventEmitter::listener_list_t> & EventEmitter::listeners( ) {
				return *m_listeners;
			}

			std::unordered_map<std::string, EventEmitter::listener_list_t> const & EventEmitter::listeners( ) const {
				return *m_listeners;
			}

			std::vector<std::string> const & EventEmitter::valid_events( ) const {
				static auto const result = std::vector < std::string > { "newListener", "removeListener", "error" };
				return result;
			}

			bool EventEmitter::at_max_listeners( std::string event ) {
				auto result = 0 != m_max_listeners;
				result &= listeners( )[event].size( ) >= m_max_listeners;
				return result;				
			}

			EventEmitter::EventEmitter( ) :m_listeners( std::make_shared<listeners_t>( ) ), m_max_listeners( 10 ), m_mutex( std::make_shared<std::mutex>( ) ) { }

			EventEmitter::EventEmitter( EventEmitter && other ): m_listeners( std::move( other.m_listeners ) ), m_max_listeners( std::move( other.m_max_listeners ) ), m_mutex( std::move( other.m_mutex ) ) { }

			EventEmitter& EventEmitter::operator=( EventEmitter && rhs ) {
				m_listeners = std::move( rhs.m_listeners );
				m_max_listeners = std::move( rhs.m_max_listeners );
				m_mutex = std::move( rhs.m_mutex );
				return *this;
			}

			void EventEmitter::swap( EventEmitter& rhs ) {
				using std::swap;
				swap( m_listeners, rhs.m_listeners );
				swap( m_max_listeners, rhs.m_max_listeners );
				swap( m_mutex, rhs.m_mutex );
			}

			EventEmitter::~EventEmitter( ) { }
			
			EventEmitter& EventEmitter::remove_listener( std::string event, callback_id_t id ) {
				std::lock_guard<std::mutex> listener_lock( *m_mutex );
				daw::algorithm::erase_remove_if( listeners( )[event], [&]( std::pair<bool, Callback> const & item ) {
					if( item.second.id( ) == id ) {
						// TODO verify if this needs to be outside loop
						emit( "removeListener", event, item.second );
						return true;
					}
					return false;
				} );
				return *this;
			}

			EventEmitter& EventEmitter::on_newListener( std::function<void( std::string, Callback )> listener ) {
				add_listener( "newListener", listener );
				return *this;
			}

			EventEmitter& EventEmitter::on_removeListener( std::function<void( std::string, Callback )> listener ) {
				add_listener( "removeListener", listener );
				return *this;
			}
			
			EventEmitter& EventEmitter::on_error( std::function<void( base::Error )> listener ) {
				add_listener( "error", listener );
				return *this;
			}

			EventEmitter& EventEmitter::once_newListener( std::function<void( std::string, Callback )> listener ) {
				add_listener( "newListener", listener, true );
				return *this;
			}
			EventEmitter& EventEmitter::once_removeListener( std::function<void( std::string, Callback )> listener ) {
				add_listener( "removeListener", listener, true );
				return *this;
			}

			EventEmitter& EventEmitter::once_error( std::function<void( base::Error )> listener ) {
				add_listener( "error", listener, true );
				return *this;
			}

			EventEmitter& EventEmitter::remove_listener( std::string event, Callback listener ) {
				return remove_listener( event, listener.id( ) );
			}

			EventEmitter& EventEmitter::remove_all_listeners( ) {
				listeners( ).clear( );
				return *this;
			}

			EventEmitter& EventEmitter::remove_all_listeners( std::string event ) {
				listeners( )[event].clear( );
				return *this;
			}

			EventEmitter& EventEmitter::set_max_listeners( size_t max_listeners ) {
				m_max_listeners = max_listeners;
				return *this;
			}

			EventEmitter::listener_list_t EventEmitter::listeners( std::string event ) {
				return listeners( )[event];
			}

			EventEmitter::listener_list_t const EventEmitter::listeners( std::string event ) const {
				return listeners( ).at( event );
			}

			size_t EventEmitter::listener_count( std::string event ) {
				return listeners( event ).size( );
			}


		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
