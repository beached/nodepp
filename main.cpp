

// #include "lib_http.h"
// 
// using namespace daw::nodepp::lib;

#include <boost/any.hpp>
#include <cstdint>
#include <functional>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include "range_algorithm.h"

template<typename...Args>
std::function<Args...> make_function( std::function<void( Args&&... )> func ) {
	return func;
}

template<typename EventEnum>
class EventEmitter {
	static_assert(std::is_enum<EventEnum>::value == true, "EventEmitter requires an enum class as template parameter");

	std::unordered_map<EventEnum, std::vector<boost::any>> m_listeners;
public:
	template<typename... Args>
	EventEmitter& add_listener( EventEnum event, std::function<void( Args... )> listener ) {
		m_listeners[event].emplace_back( listener );
		return *this;
	}

	template<typename... Args>
	EventEmitter& emit( EventEnum event, Args&&... args ) {
		using callback_t = std::function < void( Args... ) >;
		auto callbacks = m_listeners[event];
		for( auto it = std::begin( callbacks ); it != std::end( callbacks ); ++ it ) {
			try {
				auto const & any_val = *it;
				auto callback = boost::any_cast<callback_t>(any_val);
				callback( std::forward<Args>( args )... );
			} catch( boost::bad_any_cast bac ) {
				throw std::runtime_error( bac.what( ) );
			} catch( ... ) {
				throw std::runtime_error( "Error while running callback" );
				// TODO something
			}
		}
		return *this;
	}

};
#include <iostream>

void f( int a ) { std::cout << 2 * a << std::endl; }



int main( int, char const ** ) {
	enum class blah { a = 0, b, c };
	EventEmitter<blah> test;
	auto g = []( int a ) {
		std::cout << a << std::endl;
	};

	std::function<void( int )> h = g;

	test.add_listener( blah::b, h );
	//test.add_listener( blah::b, std::function < void( int ) > { f } );
// 	test.add_listener( blah::c, []( int a ) {
// 		std::cout << a << std::endl;
// 	} );
	test.emit<int>( blah::b, 500 );
// 	auto server = http::create_server( []( http::Request req, http::Response resp ) {
// 
// 	} ).listen( 8080 );
	

	system("pause");
	return EXIT_SUCCESS;
}
