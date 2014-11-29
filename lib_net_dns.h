#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <functional>

#include "base_handle.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				class NetDNS {
					boost::asio::ip::tcp::resolver m_resolver;					
				public:
					using handler_type = std::function < void( const boost::system::error_code&, boost::asio::ip::tcp::resolver::iterator ) > ;
					NetDNS( ) = default;
					~NetDNS( ) = default;
					NetDNS( NetDNS const & ) = delete;
					NetDNS& operator=(NetDNS const & rhs) = delete;
					NetDNS( NetDNS&& other );
					NetDNS& operator=(NetDNS && rhs);

					NetDNS& resolve4( std::string address, handler_type handler );
					NetDNS& resolve6( std::string address, handler_type handler );
					NetDNS& resolve_mx( std::string address, handler_type handler );
					NetDNS& resolve_txt( std::string address, handler_type handler );
					NetDNS& resolve_srv( std::string address, handler_type handler );
					NetDNS& resolve_ns( std::string address, handler_type handler );
					NetDNS& resolve_cname( std::string address, handler_type handler );
					NetDNS& reverese( std::string ip, handler_type handler );


				};	// class NetDNS
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
