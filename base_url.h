#pragma once

#include <boost/optional.hpp>
#include <map>
#include <string>

namespace daw {
	namespace nodepp {
		namespace base {
			namespace url_details {
				struct UserInfo {
					std::string username;
					std::string password;
				};

				struct UrlAuthority {
					boost::optional<UserInfo> user_info;
					std::string host;
					boost::optional<std::string> port;
				};
			}

			struct Url {
				using query_t = std::map < std::string, std::string > ;
				boost::optional< std::string > scheme;
				boost::optional<url_details::UrlAuthority> authority;
				boost::optional< std::string > path;
				boost::optional< query_t > queries;
				boost::optional<std::string> fragment;
			};


			//std::shared_ptr<Url> parse_url( daw::nodepp::base::data_t::iterator first, daw::nodepp::base::data_t::iterator last );

		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
