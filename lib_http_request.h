#pragma once
#include <boost/optional.hpp>
//#include "daw_optional.h"
#include <cstdint>
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

				std::string http_request_method_as_string( HttpClientRequestMethod method );

				struct HttpUrl {
					std::string path;
					boost::optional<std::string> query;
				};

				struct HttpRequestLine {
					HttpClientRequestMethod method;
					HttpUrl url;
					std::string version;
				};

				namespace impl {
					using namespace daw::nodepp;
					struct HttpClientRequestImpl {
						using headers_t = std::unordered_map < std::string, std::string > ;
						lib::http::HttpRequestLine request;
						headers_t headers;
					};
				}

				using HttpClientRequest = std::shared_ptr < impl::HttpClientRequestImpl > ;

				HttpClientRequest parse_http_request( daw::nodepp::base::data_t::iterator first, daw::nodepp::base::data_t::iterator last );

			} // namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
