#include <boost/asio/ip/tcp.hpp>
#include <functional>

#include "base_event_emitter.h"
#include "base_handle.h"
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


				NetDns::NetDns( ) : m_resolver( daw::make_unique<boost::asio::ip::tcp::resolver>( base::Handle::get( ) ) ) { }
				NetDns::NetDns( NetDns&& other ): m_resolver( std::move( other.m_resolver ) ) { }
				NetDns& NetDns::operator=(NetDns && rhs) { 
					if( this != &rhs ) {
						m_resolver = std::move( rhs.m_resolver );
					}
					return *this;
				}

				NetDns& NetDns::resolve( std::string const & address ) { 
					auto query = tcp::resolver::query( address, "" );
					m_resolver->async_resolve( query, [&]( boost::system::error_code err, boost::asio::ip::tcp::resolver::iterator it ) {
						this->emit<std::decay<boost::system::error_code>::type, std::decay<boost::asio::ip::tcp::resolver::iterator>::type>( "resolved", std::move( err ), std::move( it ) );
					} );
					return *this;
				}

				NetDns& NetDns::resolve_mx( std::string address ) { throw std::runtime_error( "Method Not Implemented" ); }
				NetDns& NetDns::resolve_txt( std::string address ) { throw std::runtime_error( "Method Not Implemented" ); }
				NetDns& NetDns::resolve_srv( std::string address ) { throw std::runtime_error( "Method Not Implemented" ); }
				NetDns& NetDns::resolve_ns( std::string address ) { throw std::runtime_error( "Method Not Implemented" ); }
				NetDns& NetDns::resolve_cname( std::string address ) { throw std::runtime_error( "Method Not Implemented" ); }

			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
