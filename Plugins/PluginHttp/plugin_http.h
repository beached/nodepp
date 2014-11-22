#pragma once

#include <cinttypes>
#include <functional>
#include <string>


#include "event_listener.h"
#include "plugin_base.h"


namespace daw {
	namespace nodepp {
		namespace plugins {
			// TODO START
			class HttpRequest { };
			class HttpResponse { };
			class Socket { };
			// END TODO

			class HttpEventRequest: public daw::nodepp::base::IEvent<HttpEventRequest> {
				struct CallbackArgs {
					HttpRequest req;
					HttpResponse resp;
				};

				static CallbackArgs parse_args( ... );
			public:
				virtual std::string name( ) const override;
				virtual HttpEventRequest& on( std::function<void( ... )> const & call_back ) override;
				virtual HttpEventRequest& once( std::function<void( ... )> const & call_back ) override;
				virtual HttpEventRequest& removeListener( std::function<void( ... )> const & call_back ) override;
				virtual HttpEventRequest& removeAllListeners( ) override;
				virtual std::vector<HttpEventRequest>& listeners( ) override;
				virtual std::vector<HttpEventRequest> const & listeners( ) const override;
				virtual bool emit( ... ) override;
				virtual size_t listenerCount( ) override;
				virtual void newListener( std::function<void( HttpEventRequest const & )> ) override;
				virtual void removeListener( std::function<void( HttpEventRequest const & )> ) override;
				virtual ~HttpEventRequest( );				
			};

			class HttpEventConnection: public daw::nodepp::base::IEvent < HttpEventConnection > {
				struct CallbackArgs {
					Socket socket;
				};

				static CallbackArgs parse_args( ... );
			public:
				virtual std::string name( ) const override;
				virtual HttpEventConnection& on( std::function<void( ... )> const & call_back ) override;
				virtual HttpEventConnection& once( std::function<void( ... )> const & call_back ) override;
				virtual HttpEventConnection& removeListener( std::function<void( ... )> const & call_back ) override;
				virtual HttpEventConnection& removeAllListeners( ) override;
				virtual std::vector<HttpEventConnection>& listeners( ) override;
				virtual std::vector<HttpEventConnection> const & listeners( ) const override;
				virtual bool emit( ... ) override;
				virtual size_t listenerCount( ) override;
				virtual void newListener( std::function<void( HttpEventConnection const & )> ) override;
				virtual void removeListener( std::function<void( HttpEventConnection const & )> ) override;
				virtual ~HttpEventConnection( );
			};

			class HttpServer: public IPlugin {
			public:
				virtual std::string name( ) const override;
				virtual int64_t version( ) const override;

				enum class EventType: int32_t { request, connection, close, checkContinue, connect, upgrade, clientError };

				static HttpServer create( );

				HttpEventRequest request;				
			};
		}	// namespace plugins
	}	// namespace nodepp
}	// namespace daw
