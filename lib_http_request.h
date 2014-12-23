#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>
#include <utility>
#include "base_types.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				enum class HttpRequestMethod { Options, Get, Head, Post, Put, Delete, Trace, Connect };
				
				std::string http_request_method_as_string( HttpRequestMethod method );

				struct HttpRequestLine {
					HttpRequestMethod method;
					std::string url;
					std::string version;
				};

				struct HttpClientRequest {
					using headers_t = std::unordered_map < std::string, std::string > ;
					HttpRequestLine request;
					headers_t headers;
				};

				std::shared_ptr<HttpClientRequest> parse_http_request( daw::nodepp::base::data_t::iterator first, daw::nodepp::base::data_t::iterator last );

			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
