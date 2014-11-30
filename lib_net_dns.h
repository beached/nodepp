#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdint>
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
					/*using handler_type = std::function < void( const boost::system::error_code&, boost::asio::ip::tcp::resolver::iterator ) >;*/
					using handler_argument_t = boost::asio::ip::tcp::resolver::iterator;
				private:
					std::unique_ptr<boost::asio::ip::tcp::resolver> m_resolver;					
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
					// void(boost::system::error_code, boost::asio::ip::tcp::resolver::iterator)
					NetDns& resolve( std::string const& address );
					NetDns& resolve( std::string const& address, uint16_t port );
					template<typename Listener>
					NetDns& on( std::string event, Listener listener ) {
						add_listener( event, listener );
						return *this;
					}

					template<typename Listener>
					NetDns& once( std::string event, Listener listener ) {
						add_listener( event, listener, true );
						return *this;
					}

				};	// class NetDns
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
