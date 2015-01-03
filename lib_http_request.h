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

				struct HttpUrl {
					std::string path;
					boost::optional<std::string> query;
				};
				std::ostream& operator<<(std::ostream& os, HttpUrl const & url);

				struct HttpRequestLine {
					HttpClientRequestMethod method;
					HttpUrl url;
					std::string version;
				};
				std::ostream& operator<<(std::ostream& os, HttpRequestLine const & request);

				namespace impl {
					using namespace daw::nodepp;
					struct HttpClientRequestImpl {
						using headers_t = std::unordered_map < std::string, std::string > ;
						lib::http::HttpRequestLine request;
						headers_t headers;
					};

					std::ostream& operator<<(std::ostream& os, HttpClientRequestImpl const & req);
				}	// namespace impl								

				

				using HttpClientRequest = std::shared_ptr < impl::HttpClientRequestImpl > ;

				HttpClientRequest parse_http_request( daw::nodepp::base::data_t::iterator first, daw::nodepp::base::data_t::iterator last );

			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw

