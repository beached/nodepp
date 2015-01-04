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
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				enum class HttpClientRequestMethod { Options, Get, Head, Post, Put, Delete, Trace, Connect, Any };

				std::ostream& operator<<(std::ostream& os, HttpClientRequestMethod const method);
				std::string to_string( HttpClientRequestMethod method );

				struct HttpUrlQueryPair {
					std::string name;
					boost::optional<std::string> value;
					std::string serialize_to_json( ) const;
					HttpUrlQueryPair( ) = default;
					inline HttpUrlQueryPair( std::pair<std::string, boost::optional<std::string>> const & vals ) : name( vals.first ), value( vals.second ) { }
				};

				struct HttpUrl {					
					std::string path;
					boost::optional<std::vector<HttpUrlQueryPair>> query;
					std::string serialize_to_json( ) const;
				};

				struct HttpRequestLine {
					HttpClientRequestMethod method;
					HttpUrl url;
					std::string version;
					std::string serialize_to_json( ) const;
				};

				namespace impl {
					using namespace daw::nodepp;
					struct HttpClientRequestImpl {
						using headers_t = std::unordered_map < std::string, std::string > ;
						lib::http::HttpRequestLine request;
						headers_t headers;
						std::string serialize_to_json( ) const;
					};

				}	// namespace impl								



				using HttpClientRequest = std::shared_ptr < impl::HttpClientRequestImpl > ;

				HttpClientRequest parse_http_request( daw::nodepp::base::data_t::iterator first, daw::nodepp::base::data_t::iterator last );

			} // namespace http
		}	// namespace lib
		namespace base {
			namespace json {
				std::string json_value( std::string const & name, daw::nodepp::lib::http::HttpClientRequestMethod method );
			}	// namespace json
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw

