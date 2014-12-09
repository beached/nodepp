#include <boost/shared_ptr.hpp>
#include <cinttypes>
#include <iterator>
#include <string>
#include <utility>

#include "base_event_emitter.h"
#include "base_url.h"
#include "lib_http_incoming_request.h"
#include "lib_http_server.h"
#include "lib_net_server.h"
#include "range_algorithm.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				namespace {
					inline bool is_space( char chr ) {
						return 32 == chr;
					}

					template<typename Iterator, typename Pred>
					auto find_all_where( Iterator first, Iterator last, Pred predicate ) -> std::vector<Iterator> {
						std::vector<Iterator> results;
						for( auto it = first; it != last; ++it ) {
							if( predicate( *it ) ) {
								results.push_back( it );
							}
						}
						return results;
					}

					template<typename T, typename Pred>
					auto find_all_where( T const & values, Pred predicate ) -> std::vector < decltype(std::begin( values )) > {
						return find_all_where( std::begin( values ), std::end( values ), predicate );
					}					

					template<typename Iterator>
					bool equal( Iterator first, Iterator last, std::string const & value ) {
						if( static_cast<size_t>( std::distance( first, last ) ) != value.size( ) ) {
							return false;
						}
						for( size_t off = 0; off < value.size( ); ++off ) {
							if( value[off] != *(first + static_cast<std::ptrdiff_t>( off ) ) ) {
								return false;
							}
						}
						return true;
					}

					bool set_method( std::string::const_iterator first, std::string::const_iterator last, HttpRequestMethod& method ) { 
						// REDO better, works but don't like
						if( equal( first, last, "GET" ) ) {
							method = HttpRequestMethod::Get;
						} else if( equal( first, last, "POST" ) ) {
							method = HttpRequestMethod::Post;
						} else if( equal( first, last, "PUT" ) ) {
							method = HttpRequestMethod::Put;
						} else if( equal( first, last, "HEAD" ) ) {
							method = HttpRequestMethod::Head;
						} else if( equal( first, last, "DELETE" ) ) {
							method = HttpRequestMethod::Delete;
						} else if( equal( first, last, "CONNECT" ) ) {
							method = HttpRequestMethod::Connect;
						} else if( equal( first, last, "OPTIONS" ) ) {
							method = HttpRequestMethod::Options;
						} else if( equal( first, last, "TRACE" ) ) {
							method = HttpRequestMethod::Trace;
						} else {
							method = HttpRequestMethod::Unknown;
						}
						return method != HttpRequestMethod::Unknown;
					}
				}	// namespace anonymous

				bool set_url( std::string::const_iterator first, std::string::const_iterator last, base::Url & url ) {
					try {
						url = std::string( first, last );
					} catch( std::exception const & ) {
						return false;
					}
					return true;
				}

				bool set_version( std::string::const_iterator first, std::string::const_iterator last, HttpVersion & version ) {
					try {
						version = std::string( first, last );
					} catch( std::exception const & ) {
						return false;
					}
					return true;
				}

				class RequestLine {
					HttpRequestMethod m_method;
					base::Url m_url;
					HttpVersion m_version;
					bool m_is_valid;

					void build_request_line( std::string::const_iterator first, std::string::const_iterator last ) {
						m_is_valid = true;
						auto ws = find_all_where( first, last, is_space );
						if( 2 != ws.size( ) ) {
							m_is_valid = false;
							return;
						}
						m_is_valid &= set_method( first, ws[0], m_method );
						m_is_valid &= set_url( ws[0] + 1, ws[1], m_url );
						m_is_valid &= set_version( ws[1] + 1, last, m_version );
					}
						
				public:
					RequestLine( std::string::const_iterator first, std::string::const_iterator last ):m_method( HttpRequestMethod::Unknown ), m_url( ), m_version( ), m_is_valid( true ) {
						build_request_line( first, last );
					}

					RequestLine( std::string const & line ) :m_method( HttpRequestMethod::Unknown ), m_url( ), m_version( ), m_is_valid( true ) {
						build_request_line( std::begin( line ), std::end( line ) );
					}

					bool is_valid( ) const { return m_is_valid; }

					HttpRequestMethod const & method( ) const { return m_method; }

					base::Url const & url( ) const { return m_url; }

					HttpVersion const & version( ) const { return m_version; }
				};

				HttpServer::HttpServer( ) : base::EventEmitter( ), m_netserver( ) { }

				HttpServer::HttpServer( HttpServer&& other ) : base::EventEmitter( std::move( other ) ), m_netserver( std::move( other.m_netserver ) ) { }

				HttpServer& HttpServer::operator=(HttpServer&& rhs) { 
					if( this != &rhs ) {
						m_netserver = std::move( rhs.m_netserver );
					}
					return *this;
				}
				
				HttpServer::~HttpServer( ) {  }				
				
				std::vector<std::string> const & HttpServer::valid_events( ) const {
					static auto const result = [&]( ) {
						std::vector<std::string> local{ "request", "connection", "close", "checkContinue", "connect", "upgrade", "clientError", "listening" };
						return local;
					}();
					return result;
				}

				template<typename Iterator>
				Iterator advance( Iterator it, Iterator last, typename Iterator::difference_type how_far ) {
					auto result = it;
					while( result != last && std::distance( it, result ) < how_far ) { ++it; }
					return it;
				}

				template<typename Iterator>
				Iterator find_buff( Iterator first, Iterator last, std::string const & key ) {
					auto it = advance( first, last, static_cast<typename Iterator::difference_type>( key.size( ) ) );
					if( it == last ) {
						return last;
					}
										
					for( ; it != last; ++it, ++first ) {
						if( equal( first, it, key ) ) {
							return first;
						}
					}
					return last;
				}

				void HttpServer::handle_connection( std::shared_ptr<lib::net::NetSocket> socket_ptr ) {
					using match_iterator_t = lib::net::NetSocket::match_iterator_t;
					socket_ptr->write( "Go Away\r\n\r\n" );
					socket_ptr->set_read_predicate( []( match_iterator_t begin, match_iterator_t end ) {						
						auto it = find_buff( begin, end, "\r\n\r\n" );
						return std::make_pair( it, it != end );
					} );
					socket_ptr->on_data( []( std::shared_ptr<daw::nodepp::base::data_t> data_buffer, bool ) {
						std::string buff( data_buffer->begin( ), data_buffer->end( ) );
						std::cout << buff;
					} ).on_end( []( ) {
						std::cout << "\n\n" << std::endl;
					} );
				}

				void HttpServer::handle_error( base::Error error ) {
					auto err = base::Error( "Child error" ).add( "where", "HttpServer::handle_error" ).set_child( error );
					emit( "error", err );
				}

				HttpServer& HttpServer::listen( uint16_t port ) {
					m_netserver.on_connection( std::bind( &HttpServer::handle_connection, this, std::placeholders::_1 ) )
						.on_error( std::bind( &HttpServer::handle_error, this, std::placeholders::_1 ) )
						.listen( port );
					return *this;
				}

				HttpServer& HttpServer::listen( uint16_t port, std::string hostname, uint16_t backlog ) { throw std::runtime_error( "Method not implemented" ); }

				HttpServer& HttpServer::listen( std::string path ) { throw std::runtime_error( "Method not implemented" ); }

				HttpServer& HttpServer::listen( base::ServiceHandle handle ) { throw std::runtime_error( "Method not implemented" ); }


				size_t& HttpServer::max_header_count( ) { throw std::runtime_error( "Method not implemented" ); }
				size_t const & HttpServer::max_header_count( ) const { throw std::runtime_error( "Method not implemented" ); }

				size_t HttpServer::timeout( ) const { throw std::runtime_error( "Method not implemented" ); }

				HttpServer& HttpServer::on_listening( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
					add_listener( "listening", listener, false );
					return *this;
				}
				HttpServer& HttpServer::once_listening( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
					add_listener( "listening", listener, true );
					return *this;
				}

				HttpServer& create_server( std::function<void( HttpClientRequest, HttpServerResponse )> listener ) {
					return HttpServer( ).on_listening( listener );
				}
			}
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
