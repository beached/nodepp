#include <boost/shared_ptr.hpp>
#include <cinttypes>
#include <string>
#include <utility>

#include "base_event_emitter.h"
#include "lib_http_server.h"
#include "lib_net_server.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				HttpServer::HttpServer( ) : lib::net::NetServer{ } { }

				HttpServer::HttpServer( HttpServer&& other ) : lib::net::NetServer{ std::move( other ) } { }

				HttpServer& HttpServer::operator=(HttpServer&& rhs) { 
					if( this != &rhs ) {

					}
					return *this;
				}
				
				HttpServer::~HttpServer( ) {  }				
				
				std::vector<std::string> const & HttpServer::valid_events( ) const {
					static auto const result = [&]( ) {
						std::vector<std::string> local{ "request", "connection", "close", "checkContinue", "connect", "upgrade", "clientError", "listening" };
						auto parent = lib::net::NetServer::valid_events( );
						return base::impl::append_vector( local, parent );
					}();
					return result;
				}

				HttpServer& HttpServer::listen( uint16_t port, std::string hostname, uint16_t backlog ) { throw std::runtime_error( __func__"Method not implemented" ); }

				HttpServer& HttpServer::listen( std::string path ) { throw std::runtime_error( __func__"Method not implemented" ); }

				HttpServer& HttpServer::listen( lib::net::NetHandle& handle ) { throw std::runtime_error( __func__"Method not implemented" ); }


				size_t& HttpServer::max_header_count( ) { throw std::runtime_error( __func__"Method not implemented" ); }
				size_t const & HttpServer::max_header_count( ) const { throw std::runtime_error( __func__"Method not implemented" ); }

				size_t HttpServer::timeout( ) const { throw std::runtime_error( __func__"Method not implemented" ); }


			}
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
