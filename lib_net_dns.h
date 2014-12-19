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
				class NetDns: public base::EventEmitter  {
				public:
					/*using handler_type = std::function < void( const boost::system::error_code&, boost::asio::ip::tcp::resolver::iterator ) >;*/
					using handler_argument_t = boost::asio::ip::tcp::resolver::iterator;
				private:
					std::unique_ptr<boost::asio::ip::tcp::resolver> m_resolver;	

					void handle_resolve( boost::system::error_code const & err, boost::asio::ip::tcp::resolver::iterator it );
				public:

					virtual std::vector<std::string> const & valid_events( ) const override;

					virtual ~NetDns( ) = default;
					NetDns( NetDns const & ) = delete;
					NetDns& operator=(NetDns const & rhs) = delete;
					
					NetDns( );
					NetDns( NetDns&& other );
					NetDns& operator=(NetDns && rhs);
					
					// Event callbacks
					
					//////////////////////////////////////////////////////////////////////////
					// Summary: resolve name or ip address and call callback of form
					// void(boost::system::error_code, boost::asio::ip::tcp::resolver::iterator)
					void resolve( std::string const& address );
					void resolve( std::string const& address, uint16_t port );
					
					
					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when name resolution is complete
					virtual void when_resolved( std::function<void( boost::asio::ip::tcp::resolver::iterator )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when name resolution is complete
					virtual void when_next_resolved( std::function<void( boost::asio::ip::tcp::resolver::iterator )> listener );

					//////////////////////////////////////////////////////////////////////////
					/// Summary: Event emitted when async resolve is complete
					virtual void emit_resolved( boost::asio::ip::tcp::resolver::iterator it );
				};	// class NetDns
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
