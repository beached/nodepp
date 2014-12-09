#include <cstdint>
#include <string>

#include "base_enoding.h"
#include "base_stream.h"
#include "lib_http.h"
#include "lib_http_headers.h"
#include "lib_http_server_response.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;

				std::vector<std::string> const & HttpServerResponse::valid_events( ) const {
					static auto const result = [&]( ) {
						static std::vector<std::string> const local{ "close" };
						auto parent = base::stream::StreamWritable::valid_events( );
						return base::impl::append_vector( local, parent );
					}();
					return result;
				}

				HttpServerResponse::HttpServerResponse( ) : base::stream::StreamWritable( ) { }

				HttpServerResponse& HttpServerResponse::write_continue( ) { throw std::runtime_error( "Method not implemented" ); }
				HttpServerResponse& HttpServerResponse::write_head( uint16_t, std::string, HttpHeaders ) { throw std::runtime_error( "Method not implemented" ); }


				uint16_t& HttpServerResponse::status_code( ) { throw std::runtime_error( "Method not implemented" ); }
				uint16_t const & HttpServerResponse::status_code( ) const { throw std::runtime_error( "Method not implemented" ); }

				void HttpServerResponse::set_header( std::string, std::string ) { throw std::runtime_error( "Method not implemented" ); }
				void HttpServerResponse::set_header( HttpHeaders ) { throw std::runtime_error( "Method not implemented" ); }

				bool HttpServerResponse::headers_sent( ) const { throw std::runtime_error( "Method not implemented" ); }

				bool& HttpServerResponse::send_date( ) { throw std::runtime_error( "Method not implemented" ); }
				bool const& HttpServerResponse::send_date( ) const { throw std::runtime_error( "Method not implemented" ); }

				HttpHeader const & HttpServerResponse::get_header( std::string ) const { throw std::runtime_error( "Method not implemented" ); }
				HttpServerResponse& HttpServerResponse::remove_header( std::string ) { throw std::runtime_error( "Method not implemented" ); }

				bool HttpServerResponse::write_chunk( std::string const &, base::Encoding const & ) { throw std::runtime_error( "Method not implemented" ); }
				bool HttpServerResponse::write_chunk( base::data_t const &, base::Encoding const & ) { throw std::runtime_error( "Method not implemented" ); }
				bool HttpServerResponse::add_trailers( HttpHeaders ) { throw std::runtime_error( "Method not implemented" ); }


				HttpServerResponse& HttpServerResponse::write( base::data_t const & ) { throw std::runtime_error( "Method not implemented" ); }
				HttpServerResponse& HttpServerResponse::write( std::string const &, base::Encoding const & ) { throw std::runtime_error( "Method not implemented" ); }

				void HttpServerResponse::end( ) { throw std::runtime_error( "Method not implemented" ); }
				void HttpServerResponse::end( base::data_t const & ) { throw std::runtime_error( "Method not implemented" ); }
				void HttpServerResponse::end( std::string const &, base::Encoding const & ) { throw std::runtime_error( "Method not implemented" ); }

				// Event callbacks
				HttpServerResponse& HttpServerResponse::on_close( std::function<void( )> listener ) {
					add_listener( "close", listener );
					return *this;
				}

				HttpServerResponse& HttpServerResponse::on_error( std::function<void( base::Error )> listener ) {
					add_listener( "error", listener );
					return *this;
				}

				HttpServerResponse& HttpServerResponse::on_finish( std::function<void( )> listener ) {
					add_listener( "finish", listener );
					return *this;
				}

				HttpServerResponse& HttpServerResponse::on_pipe( std::function<void( base::stream::StreamReadable& )> ) { throw std::runtime_error( "Method not implemented" ); }
				HttpServerResponse& HttpServerResponse::on_unpipe( std::function<void( base::stream::StreamReadable& )> ) { throw std::runtime_error( "Method not implemented" ); }

				HttpServerResponse& HttpServerResponse::once_close( std::function<void( )> listener ) {
					add_listener( "close", listener, true );
					return *this;
				}

				HttpServerResponse& HttpServerResponse::once_error( std::function<void( base::Error )> listener ) {
					add_listener( "error", listener, true );
					return *this;
				}

				HttpServerResponse& HttpServerResponse::once_finish( std::function<void( )> listener ) {
					add_listener( "finish", listener, true );
					return *this;
				}

				HttpServerResponse& HttpServerResponse::once_pipe( std::function<void( base::stream::StreamReadable& )> ) { throw std::runtime_error( "Method not implemented" ); }
				HttpServerResponse& HttpServerResponse::once_unpipe( std::function<void( base::stream::StreamReadable& )> ) { throw std::runtime_error( "Method not implemented" ); }


			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
