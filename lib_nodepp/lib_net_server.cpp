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

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <memory>
#include <utility>

#include "base_event_emitter.h"
#include "base_service_handle.h"
#include "base_types.h"
#include "lib_net_server.h"
#include "lib_net_socket_stream.h"
#include <daw/make_unique.h>
#include <daw/daw_range_algorithm.h>

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace net {
				namespace impl {
					using namespace daw::nodepp;
					using namespace boost::asio::ip;

					NetServerImpl::NetServerImpl( base::EventEmitter emitter ):
						daw::nodepp::base::StandardEvents <NetServerImpl>( std::move( emitter ) ),
						m_acceptor( std::make_shared<boost::asio::ip::tcp::acceptor>( base::ServiceHandle::get( ) ) ),
						m_context( nullptr ) { }

					NetServerImpl::NetServerImpl( boost::asio::ssl::context::method method, daw::nodepp::base::EventEmitter emitter ):
						daw::nodepp::base::StandardEvents <NetServerImpl>( std::move( emitter ) ),
						m_acceptor( std::make_shared<boost::asio::ip::tcp::acceptor>( base::ServiceHandle::get( ) ) ),
						m_context( std::make_shared <boost::asio::ssl::context>( method ) ) { }

					NetServerImpl::~NetServerImpl( ) { }

					boost::asio::ssl::context & NetServerImpl::ssl_context( ) {
						return *m_context;
					}

					boost::asio::ssl::context const & NetServerImpl::ssl_context( ) const {
						return *m_context;
					}

					bool NetServerImpl::using_ssl( ) const {
						return static_cast<bool>(m_context);
					}

					void NetServerImpl::listen( uint16_t port ) {
						auto endpoint = EndPoint( boost::asio::ip::tcp::v4( ), port );
						m_acceptor->open( endpoint.protocol( ) );
						m_acceptor->set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ) );
						m_acceptor->bind( endpoint );
						m_acceptor->listen( 511 );
						start_accept( );
						emit_listening( std::move( endpoint ) );
					}

					void NetServerImpl::listen( uint16_t, std::string hostname, uint16_t ) {
						throw std::runtime_error( "Method not implemented" );
					}

					void NetServerImpl::listen( std::string ) {
						throw std::runtime_error( "Method not implemented" );
					}

					void NetServerImpl::close( ) {
						throw std::runtime_error( "Method not implemented" );
					}

					daw::nodepp::lib::net::NetAddress const & NetServerImpl::address( ) const {
						throw std::runtime_error( "Method not implemented" );
					}

					void NetServerImpl::set_max_connections( uint16_t ) {
						throw std::runtime_error( "Method not implemented" );
					}

					void NetServerImpl::get_connections( std::function<void( base::Error err, uint16_t count )> callback ) {
						throw std::runtime_error( "Method not implemented" );
					}

					// Event callbacks
					NetServerImpl& NetServerImpl::on_connection( std::function<void( NetSocketStream socket )> listener ) {
						emitter( )->add_listener( "connection", listener );
						return *this;
					}

					NetServerImpl& NetServerImpl::on_next_connection( std::function<void( NetSocketStream socket_ptr )> listener ) {
						emitter( )->add_listener( "connection", listener, true );
						return *this;
					}

					NetServerImpl& NetServerImpl::on_listening( std::function<void( EndPoint )> listener ) {
						emitter( )->add_listener( "listening", listener );
						return *this;
					}

					NetServerImpl& NetServerImpl::on_next_listening( std::function<void( )> listener ) {
						emitter( )->add_listener( "listening", listener, true );
						return *this;
					}

					NetServerImpl& NetServerImpl::on_closed( std::function<void( )> listener ) {
						emitter( )->add_listener( "closed", listener, true );
						return *this;
					}

					void NetServerImpl::handle_accept( std::weak_ptr<NetServerImpl> obj, NetSocketStream&& socket, base::ErrorCode const & err ) {
						auto msocket = daw::as_move_capture( std::move( socket ) );
						run_if_valid( obj, "Exception while accepting connections", "NetServerImpl::handle_accept", [msocket, &err]( NetServer self ) mutable {
							if( !err ) {
								try {
									self->emit_connection( msocket.move_out( ) );
								} catch( ... ) {
									self->emit_error( std::current_exception( ), "Running connection listeners", "NetServerImpl::listen#emit_connection" );
								}
							} else {
								self->emit_error( err, "NetServerImpl::listen" );
							}
							self->start_accept( );
						} );
					}

					namespace {
						template<typename Handler>
						void async_accept( std::shared_ptr<boost::asio::ip::tcp::acceptor> & acceptor, boost::asio::ip::tcp::socket & socket, Handler handler ) {
							acceptor->async_accept( socket, handler );
						}

						template<typename Handler>
						void async_accept( std::shared_ptr<boost::asio::ip::tcp::acceptor> & acceptor, boost::asio::ssl::stream<boost::asio::ip::tcp::socket> & socket, Handler handler ) {
							acceptor->async_accept( socket.next_layer( ), handler );
						}
					}

					void NetServerImpl::start_accept( ) {
						auto socket_sp = create_net_socket_stream( m_context );
						socket_sp->socket( ).init( );
						auto boost_socket = socket_sp->socket( );
						auto socket = as_move_capture( std::move( socket_sp ) );

						std::weak_ptr<NetServerImpl> obj = this->get_ptr( );

						m_acceptor->async_accept( boost_socket->next_layer( ), [obj, socket]( base::ErrorCode const & err ) mutable {
							handle_accept( obj, socket.move_out( ), err );
						} );
					}

					void NetServerImpl::emit_connection( NetSocketStream socket ) {
						emitter( )->emit( "connection", std::move( socket ) );
					}

					void NetServerImpl::emit_listening( EndPoint endpoint ) {
						emitter( )->emit( "listening", std::move( endpoint ) );
					}

					void NetServerImpl::emit_closed( ) {
						emitter( )->emit( "closed" );
					}
				}	// namespace impl

				NetServer create_net_server_nossl(daw::nodepp::base::EventEmitter emitter) {
					auto tmp = new impl::NetServerImpl( boost::asio::ssl::context::tlsv12_server, std::move( emitter ) );
					return NetServer( tmp );
				}

				NetServer create_net_server( base::EventEmitter emitter ) {
					auto tmp = new impl::NetServerImpl( std::move( emitter ) );
					return NetServer( tmp );
				}

				NetServer create_net_server( boost::asio::ssl::context::method ctx_method, daw::nodepp::base::EventEmitter emitter ) {
					auto tmp = new impl::NetServerImpl( ctx_method, std::move( emitter ) );
					return NetServer( tmp );
				}
			}	// namespace net
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw

