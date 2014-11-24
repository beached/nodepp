
#include <cstdint>
#include <string>

#include "lib_event.h"
#include "lib_net_handle.h"
#include "lib_types.h"
#include "lib_net_socket.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				class SocketImpl {
				public:
					Socket::events_t events;

					SocketImpl( ) : events{ } {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					SocketImpl( options_t options ) : events{ } {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}
					
					SocketImpl( SocketImpl const & ) = delete;
					SocketImpl& operator=( SocketImpl const & ) = delete;
					~SocketImpl( ) { }


					void connect( uint16_t port, std::string host, Socket::events_t::callback_t_connect callback ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}
					void connect( std::string path, Socket::events_t::callback_t_connect callback ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					size_t& buffer_size( ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}
					size_t const & buffer_size( ) const {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					void set_encoding( encoding_t encoding ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					bool write( Socket::data_t data, encoding_t const & encoding, Socket::events_t::callback_t_drain callback ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					void end( Socket::data_t data, encoding_t const & encoding ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					void destroy( ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}
					void pause( ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}
					void resume( ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					void set_timeout( int32_t value, Socket::events_t::callback_t_timeout callback ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					void set_no_delay( bool noDelay ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					void set_keep_alive( bool keep_alive, int32_t initial_delay ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					Address const & address( ) const {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					void unref( ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					void ref( ) {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					std::string remote_address( ) const {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					uint16_t local_port( ) const {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					size_t bytes_read( ) const {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}

					size_t bytes_written( ) const {
						// TODO
						throw std::runtime_error( "Method not implemented" );
					}
				};

				Socket::Socket( ) : m_impl{ std::make_shared<SocketImpl>( ) } { }
				Socket::Socket( options_t options ) : m_impl{ std::make_shared<SocketImpl>( options ) } { }

				Socket::Socket( Socket&& other ) : m_impl{ std::move( other.m_impl ) } { }

				Socket& Socket::operator=(Socket&& rhs) {
					if( this != &rhs ) {
						m_impl = std::move( rhs.m_impl );
					}
					return *this;
				}

				Socket::~Socket( ) { }

				Socket::events_t& Socket::events( ) { return m_impl->events; }
				Socket::events_t const& Socket::events( ) const { return m_impl->events; }

				Socket& Socket::connect( uint16_t port, std::string host, events_t::callback_t_connect callback ) { m_impl->connect( port, host, callback ); return *this; }

				Socket& Socket::connect( std::string path, events_t::callback_t_connect callback ) { m_impl->connect( path, callback ); return *this; }

				size_t& Socket::buffer_size( ) { return m_impl->buffer_size( ); }

				size_t const & Socket::buffer_size( ) const { return m_impl->buffer_size( ); }

				Socket& Socket::set_encoding( encoding_t encoding ) { m_impl->set_encoding( encoding ); return *this; }

				bool Socket::write( Socket::data_t data, encoding_t const & encoding, Socket::events_t::callback_t_drain callback ) { return m_impl->write( data, encoding, callback ); }

				Socket& Socket::end( data_t data, encoding_t const & encoding ) { m_impl->end( data, encoding ); return *this; }

				Socket& Socket::destroy( ) { m_impl->destroy( ); return *this; }
				Socket& Socket::pause( ) { m_impl->pause( ); return *this; }
				Socket& Socket::resume( ) { m_impl->resume( ); return *this; }

				Socket& Socket::set_timeout( int32_t value, events_t::callback_t_timeout callback ) { m_impl->set_timeout( value, callback );  return *this; }
				Socket& Socket::set_no_delay( bool no_delay ) { m_impl->set_no_delay( no_delay ); return *this; }
				Socket& Socket::set_keep_alive( bool keep_alive, int32_t initial_delay ) { m_impl->set_keep_alive( keep_alive, initial_delay ); return *this; }

				Address const & Socket::address( ) const { return m_impl->address( ); }

				Socket& Socket::unref( ) { m_impl->unref( ); return *this; }
				Socket& Socket::ref( ) { m_impl->ref( ); return *this; }

				std::string Socket::remote_address( ) const { return m_impl->remote_address( ); }

				uint16_t Socket::local_port( ) const { return m_impl->local_port( ); }

				size_t Socket::bytes_read( ) const { return m_impl->bytes_read( ); }

				size_t Socket::bytes_written( ) const { return m_impl->bytes_written( ); }
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
