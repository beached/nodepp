#include <boost/asio/ip/tcp.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdint>
#include <functional>

#include "base_event_emitter.h"
#include "base_error.h"
#include "base_service_handle.h"
#include "lib_net_dns.h"
#include "make_unique.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				using namespace boost::asio::ip;
				using namespace daw::nodepp;

				NetDns::NetDns( base::SharedEventEmitter emitter ) :
					base::StandardEvents<NetDns>( emitter ),
					m_resolver( daw::make_unique<boost::asio::ip::tcp::resolver>( base::ServiceHandle::get( ) ) ),
					m_emitter( emitter ) { }

				NetDns::NetDns( NetDns&& other ):
					base::StandardEvents<NetDns>( std::move( other ) ),
					m_resolver( std::move( other.m_resolver ) ),
					m_emitter( std::move( other.m_emitter ) ) { }

				NetDns& NetDns::operator=(NetDns && rhs) {
					if( this != &rhs ) {
						m_resolver = std::move( rhs.m_resolver );
						m_emitter = std::move( rhs.m_emitter );
					}
					return *this;
				}

				std::shared_ptr<NetDns> NetDns::get_ptr( ) {
					return shared_from_this( );
				}

				void NetDns::resolve( boost::string_ref address ) {
					auto query = tcp::resolver::query( address.to_string( ), "", boost::asio::ip::resolver_query_base::numeric_host );

					auto handler = [&]( boost::system::error_code const & err, boost::asio::ip::tcp::resolver::iterator it ) {
						handle_resolve( err, std::move( it ) );
					};

					m_resolver->async_resolve( query, handler );
				}

				void NetDns::resolve( boost::string_ref address, uint16_t port ) {
					auto query = tcp::resolver::query( address.to_string( ), boost::lexical_cast<std::string>(port), boost::asio::ip::resolver_query_base::numeric_host );

					auto handler = [&]( boost::system::error_code const & err, boost::asio::ip::tcp::resolver::iterator it ) {
						handle_resolve( err, std::move( it ) );
					};

					m_resolver->async_resolve( query, handler );
				}

				void NetDns::on_resolved( std::function<void( boost::asio::ip::tcp::resolver::iterator )> listener ) {
					m_emitter->add_listener( "resolved", listener );
				}

				void NetDns::on_next_resolved( std::function<void( boost::asio::ip::tcp::resolver::iterator )> listener ) {
					m_emitter->add_listener( "resolved", listener, true );
				}

				void NetDns::handle_resolve( boost::system::error_code const & err, boost::asio::ip::tcp::resolver::iterator it ) {
					if( !err ) {
						emit_resolved( std::move( it ) );
					} else {
						emit_error( err, "NetDns::resolve" );
					}
				}

				void NetDns::emit_resolved( boost::asio::ip::tcp::resolver::iterator it ) {
					m_emitter->emit( "resolved", std::move( it ) );
				}

			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
