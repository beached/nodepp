#pragma once

#include <algorithm>

namespace daw {
	namespace algorithm {
		using std::begin;
		using std::end;

		template<typename Container>
		Container& sort( Container& container ) {
			std::sort( begin( container ), end( container ) );
			return container;
		}

		template<typename Container, typename UnaryPredicate>
		Container& sort( Container& container, UnaryPredicate pred ) {
			std::sort( begin( container ), end( container ), pred );
			return container;
		}

		template<typename Container, typename Value>
		auto find( Container const & container, Value const & value ) -> decltype(end( container )) {
			return std::find( begin( container ), end( container ), value );
		}

		template<typename Container, typename UnaryPredicate>
		auto find_if( Container const & container, UnaryPredicate pred ) -> decltype(end( container )) {
			return std::find_if( begin( container ), end( container ), pred );
		}

		template<typename Container, typename Value>
		auto erase_remove( Container& container, Value const & value ) -> decltype( container.erase( end( container ), end( container ) ) ) {
			return container.erase( std::remove( begin( container ), end( container ), value ), end( container ) );
		}

		template<typename Container, typename UnaryPredicate>
		auto erase_remove_if( Container& container, UnaryPredicate const & pred ) -> decltype(container.erase( end( container ), end( container ) )) {
			return container.erase( std::remove_if( begin( container ), end( container ), pred ), end( container ) );
		}

        template<typename Container, typename Value>
        bool contains( Container const & container, Value const & value ) {
            return std::find( std::begin( container ), std::end( container ), value ) != std::end( container );
        }

        template<typename Container, typename Value, typename UnaryPredicate>
        bool contains( Container const & container, Value const & value, UnaryPredicate pred ) {
            auto pred2 = [&value, &pred]( Value const & val ) {
                return pred( value, val );
            };
            return std::find_if( std::begin( container ), std::end( container ), pred2 ) != std::end( container );
        }



    }	// namespace algorithm
}	// namespace daw
