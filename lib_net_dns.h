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
				struct NetDns: public std::enable_shared_from_this<NetDns>, public base::StandardEvents < NetDns > {
					using handler_argument_t = boost::asio::ip::tcp::resolver::iterator;

					NetDns( base::SharedEventEmitter = base::create_shared_event_emitter( ) );
					NetDns( NetDns&& other );
					NetDns& operator=(NetDns && rhs);

					~NetDns( ) = default;
					NetDns( NetDns const & ) = delete;
					NetDns& operator=(NetDns const & rhs) = delete;

					std::shared_ptr<NetDns> get_ptr( );

					//////////////////////////////////////////////////////////////////////////
					// Summary: resolve name or ip address and call callback of form
					// void(boost::system::error_code, boost::asio::ip::tcp::resolver::iterator)
					void resolve( boost::string_ref address );
					void resolve( boost::string_ref address, uint16_t port );

					// Event callbacks

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when name resolution is complete
					virtual void on_resolved( std::function<void( boost::asio::ip::tcp::resolver::iterator )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when name resolution is complete
					virtual void on_next_resolved( std::function<void( boost::asio::ip::tcp::resolver::iterator )> listener );

				private:
					std::unique_ptr<boost::asio::ip::tcp::resolver> m_resolver;
					base::SharedEventEmitter m_emitter;

					void handle_resolve( boost::system::error_code const & err, boost::asio::ip::tcp::resolver::iterator it );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when async resolve is complete
					virtual void emit_resolved( boost::asio::ip::tcp::resolver::iterator it );
				};	// class NetDns

				using SharedNetDns = std::shared_ptr < NetDns > ;

				template<typename... Args>
				SharedNetDns create_shared_net_dns( Args... args ) {
					return SharedNetDns( new NetDns( std::forward<Args>( args )... ) );
				}

			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
