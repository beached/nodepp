#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <utility>

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				using namespace daw::nodepp;
				namespace request {

					enum class HttpRequestMethod { Options, Get, Head, Post, Put, Delete, Trace, Connect };
	
					struct HttpRequestLine {
						HttpRequestMethod method;
						std::string url;
						std::string version;
					};

					struct HttpClientRequest {
						using headers_t = std::map < std::string, std::string > ;
						HttpRequestLine request;
						headers_t headers;
					};
				}	// namespace request

				std::shared_ptr<request::HttpClientRequest> parse_http_request( unsigned char const * first, unsigned char const * last );
			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
