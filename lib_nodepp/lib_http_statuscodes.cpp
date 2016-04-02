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

#include <cstdint>
#include <unordered_map>
#include <string>
#include <utility>

#include "lib_http.h"

namespace daw {
	namespace nodepp {
		namespace lib {
			namespace http {
				std::pair<uint16_t, std::string> HttpStatusCodes( uint16_t code ) {
					static std::unordered_map<uint16_t, std::string> const status_codes = {
						{ 100, "Continue" },
						{ 101, "Switching Protocols" },
						{ 102, "Processing" },
						{ 200, "OK" },
						{ 201, "Created" },
						{ 202, "Accepted" },
						{ 203, "Non-Authoritative Information" },
						{ 204, "No Content" },
						{ 205, "Reset Content" },
						{ 206, "Partial Content" },
						{ 207, "Multi-Status" },
						{ 208, "Already Reported" },
						{ 226, "IM Used" },
						{ 300, "Multiple Choices" },
						{ 301, "Moved Permanently" },
						{ 302, "Found" },
						{ 303, "See Other" },
						{ 304, "Not Modified" },
						{ 305, "Use Proxy" },
						{ 307, "Temporary Redirect" },
						{ 308, "Permanent Redirect" },
						{ 400, "Bad Request" },
						{ 401, "Unauthorized" },
						{ 402, "Payment Required" },
						{ 403, "Forbidden" },
						{ 404, "Not Found" },
						{ 405, "Method Not Allowed" },
						{ 406, "Not Acceptable" },
						{ 407, "Proxy Authentication Required" },
						{ 408, "Request Timeout" },
						{ 409, "Conflict" },
						{ 410, "Gone" },
						{ 411, "Length Required" },
						{ 412, "Precondition Failed" },
						{ 413, "Payload Too Large" },
						{ 414, "URI Too Long" },
						{ 415, "Unsupported Media Type" },
						{ 416, "Range Not Satisfiable" },
						{ 417, "Expectation Failed" },
						{ 422, "Unprocessable Entity" },
						{ 423, "Locked" },
						{ 424, "Failed Dependency" },
						{ 426, "Upgrade Required" },
						{ 428, "Precondition Required" },
						{ 429, "Too Many Requests" },
						{ 431, "Request Header Fields Too Large" },
						{ 500, "Internal Server Error" },
						{ 501, "Not Implemented" },
						{ 502, "Bad Gateway" },
						{ 503, "Service Unavailable" },
						{ 504, "Gateway Timeout" },
						{ 505, "HTTP Version Not Supported" },
						{ 506, "Variant Also Negotiates" },
						{ 507, "Insufficient Storage" },
						{ 508, "Loop Detected" },
						{ 510, "Not Extended" },
						{ 511, "Network Authentication Required" }
					};
					{
						auto it = status_codes.find( code );
						if( it != std::end( status_codes ) ) {
							return { it->first, it->second };
						}
					}
					return { code, "Generic Error" };
				}
			}	// namespace http
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw

