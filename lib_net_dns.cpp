#include <boost/asio/ip/tcp.hpp>
#include <functional>

#include "base_event_emitter.h"
#include "base_handle.h"
#include "lib_net_dns.h"


namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				using namespace boost::asio::ip;
				using namespace daw::nodepp;
				
				std::vector<std::string> const & NetDns::valid_events( ) const {
					static auto const result = [&]( ) {
						auto local = std::vector < std::string > { "resolved", "error" };
						return base::impl::append_vector( local, base::EventEmitter::valid_events( ) );
					}();
					return result;
				}


				NetDns::NetDns( ) : m_resolver( base::Handle::get( ) ) { }
				NetDns::NetDns( NetDns&& other ): m_resolver( std::move( other.m_resolver ) ) { }
				NetDns& NetDns::operator=(NetDns && rhs) { 
					if( this != &rhs ) {
						m_resolver = std::move( rhs.m_resolver );
					}
					return *this;
				}

				NetDns& NetDns::do_lookup( std::string address, handler_type handler ) { 
					auto query = tcp::resolver::query( address, "" );
					m_resolver.async_resolve( query, handler );
					return *this;
				}

				NetDns& NetDns::resolve_mx( std::string address, handler_type handler ) { throw std::runtime_error( "Method Not Implemented" ); }
				NetDns& NetDns::resolve_txt( std::string address, handler_type handler ) { throw std::runtime_error( "Method Not Implemented" ); }
				NetDns& NetDns::resolve_srv( std::string address, handler_type handler ) { throw std::runtime_error( "Method Not Implemented" ); }
				NetDns& NetDns::resolve_ns( std::string address, handler_type handler ) { throw std::runtime_error( "Method Not Implemented" ); }
				NetDns& NetDns::resolve_cname( std::string address, handler_type handler ) { throw std::runtime_error( "Method Not Implemented" ); }

			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
