#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <functional>

#include "base_event_emitter.h"
#include "base_handle.h"
#include "utility.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				using namespace daw::nodepp;
				class NetDns: public base::EventEmitter  {
				public:
					using handler_type = std::function < void( const boost::system::error_code&, boost::asio::ip::tcp::resolver::iterator ) >;
				private:
					std::unique_ptr<boost::asio::ip::tcp::resolver> m_resolver;					
					NetDns& do_lookup( std::string address, handler_type handler );
				public:

					virtual std::vector<std::string> const & valid_events( ) const override;

					virtual ~NetDns( ) = default;
					NetDns( NetDns const & ) = delete;
					NetDns& operator=(NetDns const & rhs) = delete;
					
					NetDns( );
					NetDns( NetDns&& other );
					NetDns& operator=(NetDns && rhs);

					//////////////////////////////////////////////////////////////////////////
					// Summary: resolve name or ip address and call callback of form
					// void(boost::system::error_code const &, boost::asio::ip::tcp::resolver::iterator)
					template<typename Handler>
					NetDns& resolve( std::string const& address, Handler const & handler ) {
						return do_lookup( address, daw::make_function( handler ) );
					}
					NetDns& resolve_mx( std::string address, handler_type handler );
					NetDns& resolve_txt( std::string address, handler_type handler );
					NetDns& resolve_srv( std::string address, handler_type handler );
					NetDns& resolve_ns( std::string address, handler_type handler );
					NetDns& resolve_cname( std::string address, handler_type handler );
				};	// class NetDns
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
