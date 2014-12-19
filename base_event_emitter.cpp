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
			void EventEmitter::emit_error( base::Error error ) {
				emit( "error", std::move( error ) );
			}

			void EventEmitter::emit_error( std::string description, std::string where ) {
				base::Error err( description );
				err.add( "where", std::move( where ) );

				emit_error( std::move( err ) );
			}

			void EventEmitter::emit_error( std::string where, base::Error child ) { 
				base::Error err( "Child Error" );
				err.add( "where", std::move( where ) );
				err.child( std::move( child ) );
				
				emit_error( std::move( err ) );
			}

			void EventEmitter::emit_error( boost::system::error_code const & error, std::string where ) {
				base::Error err( error );
				err.add( "where", where );

				emit_error( std::move( err ) );
			}

			void EventEmitter::emit_error( std::exception_ptr ex, std::string description, std::string where ) {
				base::Error err( description, ex );
				err.add( "where", where );

				emit_error( std::move( err ) );
			}

			void EventEmitter::emit_new_listener( std::string event, Callback listener ) {
				emit( "newListener", std::move( event ), std::move( listener ) );
			}

			void EventEmitter::emit_remove_listener( std::string event, Callback listener ) {
				emit( "removeListener", std::move( event ), std::move( listener ) );
			}

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

			EventEmitter::EventEmitter( ) :
				m_listeners( std::make_shared<listeners_t>( ) ), 
				m_max_listeners( 10 ) { }

			EventEmitter::EventEmitter( EventEmitter && other ):
				m_listeners( std::move( other.m_listeners ) ), 
				m_max_listeners( std::move( other.m_max_listeners ) ) { }

			EventEmitter& EventEmitter::operator=( EventEmitter rhs ) {
				m_listeners = std::move( rhs.m_listeners );
				m_max_listeners = std::move( rhs.m_max_listeners );
				return *this;
			}

			void EventEmitter::swap( EventEmitter& rhs ) {
				using std::swap;
				swap( m_listeners, rhs.m_listeners );
				swap( m_max_listeners, rhs.m_max_listeners );
			}

			EventEmitter::~EventEmitter( ) { }
			
			void EventEmitter::remove_listener( std::string event, callback_id_t id ) {
				daw::algorithm::erase_remove_if( listeners( )[event], [&]( std::pair<bool, Callback> const & item ) {
					if( item.second.id( ) == id ) {
						// TODO verify if this needs to be outside loop
						emit_remove_listener( event, item.second );
						return true;
					}
					return false;
				} );
			}

			void EventEmitter::when_listener_added( std::function<void( std::string, Callback )> listener ) {
				add_listener( "newListener", listener );
			}

			void EventEmitter::when_listener_removed( std::function<void( std::string, Callback )> listener ) {
				add_listener( "removeListener", listener );
			}
			
			void EventEmitter::when_error( std::function<void( base::Error )> listener ) {
				add_listener( "error", listener );
			}

			void EventEmitter::when_next_listener_added( std::function<void( std::string, Callback )> listener ) {
				add_listener( "newListener", listener, true );
			}

			void EventEmitter::when_next_listener_removed( std::function<void( std::string, Callback )> listener ) {
				add_listener( "removeListener", listener, true );
			}

			void EventEmitter::when_next_error( std::function<void( base::Error )> listener ) {
				add_listener( "error", listener, true );
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

			EventEmitter::listener_list_t const EventEmitter::listeners( std::string event ) const {
				return listeners( ).at( event );
			}

			size_t EventEmitter::listener_count( std::string event ) {
				return listeners( event ).size( );
			}

			void EventEmitter::tap( std::function<void( std::string, size_t )> listener ) {
				add_listener( "tap", listener, false );
			}

			void EventEmitter::untap( ) {
				remove_all_listeners( "tap" );
			}

		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
