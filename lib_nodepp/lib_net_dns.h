// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdint>
#include <functional>

#include "base_event_emitter.h"
#include "base_service_handle.h"
#include <daw/daw_utility.h>

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				namespace impl {
					class NetDnsImpl;
				}

				using NetDns = std::shared_ptr <impl::NetDnsImpl>;

				NetDns create_net_dns( daw::nodepp::base::EventEmitter emitter = daw::nodepp::base::create_event_emitter( ) );
				using Resolver = boost::asio::ip::tcp::resolver;

				namespace impl {
					class NetDnsImpl final: public daw::nodepp::base::enable_shared<NetDnsImpl>, public daw::nodepp::base::StandardEvents <NetDnsImpl> {
						explicit NetDnsImpl( daw::nodepp::base::EventEmitter emitter );

					public:
						friend daw::nodepp::lib::net::NetDns daw::nodepp::lib::net::create_net_dns( daw::nodepp::base::EventEmitter );

						using handler_argument_t = Resolver::iterator;
						
						NetDnsImpl( ) = delete;
						~NetDnsImpl( );
						NetDnsImpl( NetDnsImpl const & ) = delete;
						NetDnsImpl( NetDnsImpl&& ) = default;
						NetDnsImpl& operator=( NetDnsImpl const & ) = delete;						
						NetDnsImpl& operator=( NetDnsImpl && ) = default;

						//////////////////////////////////////////////////////////////////////////
						// Summary: resolve name or ip address and call callback of form
						// void(base::ErrorCode, Resolver::iterator)
						void resolve( boost::string_ref address );
						void resolve( boost::string_ref address, uint16_t port );
						void resolve( Resolver::query & query );
						// Event callbacks

						//////////////////////////////////////////////////////////////////////////
						/// Summary: Event emitted when name resolution is complete
						NetDnsImpl& on_resolved( std::function<void( Resolver::iterator )> listener );

						//////////////////////////////////////////////////////////////////////////
						/// Summary: Event emitted when name resolution is complete
						NetDnsImpl& on_next_resolved( std::function<void( Resolver::iterator )> listener );

					private:
						std::unique_ptr<Resolver> m_resolver;

						static void handle_resolve( std::weak_ptr<NetDnsImpl> obj, base::ErrorCode const & err, Resolver::iterator it );

						//////////////////////////////////////////////////////////////////////////
						/// Summary: Event emitted when async resolve is complete
						void emit_resolved( Resolver::iterator it );
					};	// class NetDnsImpl
				}	// namespace impl
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw

