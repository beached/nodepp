#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <typeinfo>
#include <unordered_map>
#include "daw_traits.h"

template<typename T>
void test( T const & val ) {
	std::cout << "-----------------\n";
	std::cout << typeid( val ).name( ) << std::endl;
	std::cout << "begin " << daw::traits::has_begin_member<T>::value << std::endl;	
	std::cout << "end " << daw::traits::has_end_member<T>::value << std::endl;	
	std::cout << "substr " << daw::traits::has_substr_member<T>::value << std::endl;	
	std::cout << "push_back " << daw::traits::has_push_back_member_t<T>::value << std::endl;	
	std::cout << "type " << daw::traits::has_type_member<T>::value << std::endl;	
	std::cout << "container " << daw::traits::is_container_like<T>::value << std::endl;
	std::cout << "string " << daw::traits::is_string<T>::value << std::endl;
	std::cout << "is not string " << daw::traits::isnt_string<T>::value << std::endl;
	std::cout << "is map like " << daw::traits::is_map_like<T>::value << std::endl;
	std::cout << "is vector like, not string " << daw::traits::is_vector_like_not_string<T>::value << std::endl;
	std::cout << "-----------------\n";
}

int main(int, char**) {
	std::string s;
	std::vector<std::string> v;
	test( s );
	test( v );
	std::unordered_map<std::string, int> umap;
	test( umap );
	return EXIT_SUCCESS;

}
