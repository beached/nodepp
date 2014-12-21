#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdint>
#include <functional>

#include "base_event_emitter.h"
#include "base_service_handle.h"
#include "utility.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				using namespace daw::nodepp;
				namespace impl {
					class NetDnsImpl;
				}

				using NetDns = std::shared_ptr < impl::NetDnsImpl > ;

				NetDns create_net_dns( base::EventEmitter emitter = base::create_event_emitter( ) );

				namespace impl {
					class NetDnsImpl: public std::enable_shared_from_this<NetDnsImpl>, public base::StandardEvents < NetDnsImpl > {
						NetDnsImpl( base::EventEmitter );

					public:
						friend lib::net::NetDns lib::net::create_net_dns( base::EventEmitter );

						using handler_argument_t = boost::asio::ip::tcp::resolver::iterator;
							
						NetDnsImpl( NetDnsImpl&& other );
						NetDnsImpl& operator=(NetDnsImpl && rhs);
	
						~NetDnsImpl( ) = default;
						NetDnsImpl( NetDnsImpl const & ) = delete;
						NetDnsImpl& operator=(NetDnsImpl const & rhs) = delete;
	
						std::shared_ptr<NetDnsImpl> get_ptr( );
						base::EventEmitter& emitter( );
						//////////////////////////////////////////////////////////////////////////
						// Summary: resolve name or ip address and call callback of form
						// void(boost::system::error_code, boost::asio::ip::tcp::resolver::iterator)
						void resolve( boost::string_ref address );
						void resolve( boost::string_ref address, uint16_t port );
	
						// Event callbacks
	
						//////////////////////////////////////////////////////////////////////////
						/// Summary: Event emitted when name resolution is complete
						NetDnsImpl& on_resolved( std::function<void( boost::asio::ip::tcp::resolver::iterator )> listener );
	
						//////////////////////////////////////////////////////////////////////////
						/// Summary: Event emitted when name resolution is complete
						NetDnsImpl& on_next_resolved( std::function<void( boost::asio::ip::tcp::resolver::iterator )> listener );
	
					private:
						std::unique_ptr<boost::asio::ip::tcp::resolver> m_resolver;
						base::EventEmitter m_emitter;
	
						static void handle_resolve( std::shared_ptr<NetDnsImpl> self, boost::system::error_code const & err, boost::asio::ip::tcp::resolver::iterator it );
	
						//////////////////////////////////////////////////////////////////////////
						/// Summary: Event emitted when async resolve is complete
						virtual void emit_resolved( boost::asio::ip::tcp::resolver::iterator it );
					};	// class NetDnsImpl
				}	// namespace impl		

			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
