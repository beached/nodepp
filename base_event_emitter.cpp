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
			namespace impl {
				EventEmitterImpl::EventEmitterImpl( ) :
					m_listeners( std::make_shared<listeners_t>( ) ),
					m_max_listeners( 10 ),
					m_emit_depth( std::make_shared<std::atomic_int_least8_t>( 0 ) ) { }

				EventEmitterImpl::EventEmitterImpl( EventEmitterImpl && other ):
					m_listeners( std::move( other.m_listeners ) ),
					m_max_listeners( std::move( other.m_max_listeners ) ),
					m_emit_depth( std::move( other.m_emit_depth ) ) {
				}

				EventEmitterImpl& EventEmitterImpl::operator=(EventEmitterImpl && rhs) {
					if( this != &rhs ) {
						m_listeners = std::move( rhs.m_listeners );
						m_max_listeners = std::move( rhs.m_max_listeners );
						m_emit_depth = std::move( rhs.m_emit_depth );
					}
					return *this;
				}

				void EventEmitterImpl::swap( EventEmitterImpl& rhs ) {
					using std::swap;
					swap( m_listeners, rhs.m_listeners );
					swap( m_max_listeners, rhs.m_max_listeners );
					swap( m_emit_depth, rhs.m_emit_depth );
				}

				std::unordered_map<std::string, EventEmitterImpl::listener_list_t> & EventEmitterImpl::listeners( ) {
					return *m_listeners;
				}

				bool EventEmitterImpl::at_max_listeners( boost::string_ref event ) {
					auto result = 0 != m_max_listeners;
					result &= listeners( )[event.to_string()].size( ) >= m_max_listeners;
					return result;
				}

				void EventEmitterImpl::remove_listener( boost::string_ref event, callback_id_t id ) {
					daw::algorithm::erase_remove_if( listeners( )[event.to_string( )], [&]( std::pair<bool, Callback> const & item ) {
						if( item.second.id( ) == id ) {
							// TODO verify if this needs to be outside loop
							emit_listener_removed( event, item.second );
							return true;
						}
						return false;
					} );
				}

				void EventEmitterImpl::remove_listener( boost::string_ref event, Callback listener ) {
					return remove_listener( event, listener.id( ) );
				}

				void EventEmitterImpl::remove_all_listeners( ) {
					listeners( ).clear( );
				}

				void EventEmitterImpl::remove_all_listeners( boost::string_ref event ) {
					listeners( )[event.to_string( )].clear( );
				}

				void EventEmitterImpl::set_max_listeners( size_t max_listeners ) {
					m_max_listeners = max_listeners;
				}

				EventEmitterImpl::listener_list_t EventEmitterImpl::listeners( boost::string_ref event ) {
					return listeners( )[event.to_string( )];
				}

				size_t EventEmitterImpl::listener_count( boost::string_ref event ) {
					return listeners( event ).size( );
				}

//				std::shared_ptr<EventEmitterImpl> EventEmitterImpl::get_ptr( ) {
// 					return shared_from_this( );
// 				}

				void EventEmitterImpl::emit_listener_added( boost::string_ref event, Callback listener ) {
					emit( "listener_added", event, std::move( listener ) );
				}

				void EventEmitterImpl::emit_listener_removed( boost::string_ref event, Callback listener ) {
					emit( "listener_removed", event, std::move( listener ) );
				}
			}	// namespace impl

			EventEmitter create_event_emitter( ) {
				return EventEmitter( new impl::EventEmitterImpl( ) );
			}
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
