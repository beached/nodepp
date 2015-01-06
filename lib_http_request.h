#pragma once

#include <boost/optional.hpp>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "base_types.h"


namespace daw {
	namespace nodepp {
		namespace base {
			namespace json {
				struct JsonLink;
			}
		}
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				enum class HttpClientRequestMethod { Options, Get, Head, Post, Put, Delete, Trace, Connect, Any };

				std::ostream& operator<<(std::ostream& os, HttpClientRequestMethod const method);
				std::string to_string( HttpClientRequestMethod method );

				std::string value_to_json( std::string const & name, HttpClientRequestMethod method );

				struct HttpUrlQueryPair {
					std::string name;
					boost::optional<std::string> value;
					HttpUrlQueryPair( ) = default;
					inline HttpUrlQueryPair( std::pair<std::string, boost::optional<std::string>> const & vals ) : name( vals.first ), value( vals.second ) { }
				};

				std::string value_to_json( std::string const & name, HttpUrlQueryPair const & query_pair );

				struct HttpUrl {					
					std::string path;
					boost::optional<std::vector<HttpUrlQueryPair>> query;
					boost::optional<std::string> fragment;					
				};

				std::string value_to_json( std::string const & name, HttpUrl const & url );

				struct HttpRequestLine {
					HttpClientRequestMethod method;
					HttpUrl url;
					std::string version;					
				};

				std::string value_to_json( std::string const & name, HttpRequestLine const & request_line );


				namespace impl {
					using namespace daw::nodepp;
					struct HttpClientRequestImpl {
						using headers_t = std::unordered_map < std::string, std::string > ;
						lib::http::HttpRequestLine request;
						headers_t headers;
					};
					std::string value_to_json( std::string const & name, HttpClientRequestImpl const & request );


				}	// namespace impl								

				// 				struct HttpClientRequest {
				// 					HttpClientRequestMethod method;
				// 					std::string version;
				// 					std::string path;
				// 					typename impl::HttpClientRequestImpl::headers_t headers;
				// 					boost::optional<std::vector<impl::HttpUrlQueryPairImpl>> query;					
				// 				};	// struct HttpClientRequest

				using HttpClientRequest = std::shared_ptr < impl::HttpClientRequestImpl > ;

				HttpClientRequest parse_http_request( daw::nodepp::base::data_t::iterator first, daw::nodepp::base::data_t::iterator last );

			} // namespace http
		}	// namespace lib

	}	// namespace nodepp
}	// namespace daw

