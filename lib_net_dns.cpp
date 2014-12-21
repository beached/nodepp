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
				namespace impl {
					using namespace boost::asio::ip;
					using namespace daw::nodepp;
	
					NetDnsImpl::NetDnsImpl( base::EventEmitter emitter ) :
						m_resolver( daw::make_unique<boost::asio::ip::tcp::resolver>( base::ServiceHandle::get( ) ) ),
						m_emitter( emitter ) { }
	
					NetDnsImpl::NetDnsImpl( NetDnsImpl&& other ):
						m_resolver( std::move( other.m_resolver ) ),
						m_emitter( std::move( other.m_emitter ) ) { }
	
					NetDnsImpl& NetDnsImpl::operator=(NetDnsImpl && rhs) {
						if( this != &rhs ) {
							m_resolver = std::move( rhs.m_resolver );
							m_emitter = std::move( rhs.m_emitter );
						}
						return *this;
					}
	
					std::shared_ptr<NetDnsImpl> NetDnsImpl::get_ptr( ) {
						return shared_from_this( );
					}
	
					base::EventEmitter& NetDnsImpl::emitter( ) {
						return m_emitter;
					}

					void NetDnsImpl::resolve( boost::string_ref address ) {
						auto query = tcp::resolver::query( address.to_string( ), "", boost::asio::ip::resolver_query_base::numeric_host );
	
						auto handler = [&]( boost::system::error_code const & err, boost::asio::ip::tcp::resolver::iterator it ) {
							handle_resolve( get_ptr( ), err, std::move( it ) );
						};
	
						m_resolver->async_resolve( query, handler );
					}
	
					void NetDnsImpl::resolve( boost::string_ref address, uint16_t port ) {
						auto query = tcp::resolver::query( address.to_string( ), boost::lexical_cast<std::string>(port), boost::asio::ip::resolver_query_base::numeric_host );
	
						auto handler = [&]( boost::system::error_code const & err, boost::asio::ip::tcp::resolver::iterator it ) {
							handle_resolve( get_ptr( ), err, std::move( it ) );
						};
	
						m_resolver->async_resolve( query, handler );
					}
	
					NetDnsImpl& NetDnsImpl::on_resolved( std::function<void( boost::asio::ip::tcp::resolver::iterator )> listener ) {
						m_emitter->add_listener( "resolved", listener );
						return *this;
					}
	
					NetDnsImpl& NetDnsImpl::on_next_resolved( std::function<void( boost::asio::ip::tcp::resolver::iterator )> listener ) {
						m_emitter->add_listener( "resolved", listener, true );
						return *this;
					}
	
					void NetDnsImpl::handle_resolve( std::shared_ptr<NetDnsImpl> self, boost::system::error_code const & err, boost::asio::ip::tcp::resolver::iterator it ) {
						if( !err ) {
							self->emit_resolved( std::move( it ) );
						} else {
							self->emit_error( err, "NetDnsImpl::resolve" );
						}
					}
	
					void NetDnsImpl::emit_resolved( boost::asio::ip::tcp::resolver::iterator it ) {						
						m_emitter->emit( "resolved", std::move( it ) );						
					}
				}	// namespace impl

				NetDns create_net_dns( base::EventEmitter emitter ) {
					return NetDns( new impl::NetDnsImpl( std::move( emitter ) ) );
				}

			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
