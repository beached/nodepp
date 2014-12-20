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
				
				std::vector<std::string> const & NetDns::valid_events( ) const {
					static auto const result = [&]( ) {
						auto local = std::vector < std::string > { "resolved" };
						return base::impl::append_vector( local, base::EventEmitter::valid_events( ) );
					}();
					return result;
				}


				NetDns::NetDns( ) : m_resolver( daw::make_unique<boost::asio::ip::tcp::resolver>( base::ServiceHandle::get( ) ) ) { }
				NetDns::NetDns( NetDns&& other ): m_resolver( std::move( other.m_resolver ) ) { }
				NetDns& NetDns::operator=(NetDns && rhs) { 
					if( this != &rhs ) {
						m_resolver = std::move( rhs.m_resolver );
					}
					return *this;
				}

				void NetDns::emit_resolved( boost::asio::ip::tcp::resolver::iterator it ) {
					emit( "resolved", std::move( it ) );
				}				

				void NetDns::handle_resolve( boost::system::error_code const & err, boost::asio::ip::tcp::resolver::iterator it ) {
					if( !err ) {
						emit_resolved( std::move( it ) );
					} else {
						emit_error( err, "NetDns::resolve" );
					}
				}

				void NetDns::resolve( std::string const & address ) { 
					auto query = tcp::resolver::query( address, "", boost::asio::ip::resolver_query_base::numeric_host );

					auto handler = [&]( boost::system::error_code const & err, boost::asio::ip::tcp::resolver::iterator it ) {
						handle_resolve( err, std::move( it ) );
					};

					m_resolver->async_resolve( query, handler );
				}

				void NetDns::resolve( std::string const & address, uint16_t port ) {
					auto query = tcp::resolver::query( address, boost::lexical_cast<std::string>( port ), boost::asio::ip::resolver_query_base::numeric_host );

					auto handler = [&]( boost::system::error_code const & err, boost::asio::ip::tcp::resolver::iterator it ) {
						handle_resolve( err, std::move( it ) );
					};

					m_resolver->async_resolve( query, handler );
				}

				void NetDns::on_resolved( std::function<void( boost::asio::ip::tcp::resolver::iterator)> listener ) {
					add_listener( "resolved", listener );
				}

				void NetDns::on_next_resolved( std::function<void( boost::asio::ip::tcp::resolver::iterator )> listener ) {
					add_listener( "resolved", listener, true );
				}

			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
