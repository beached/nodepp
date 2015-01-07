// The MIT License (MIT)
// 
// Copyright (c) 2014-2015 Darrell Wright
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

#pragma once

#include <type_traits>
#include <utility>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <deque>

namespace daw {
	namespace traits {
		template<typename ...>
		using void_t = void;

		namespace details {
			template<typename T>
			std::false_type is_equality_comparable_impl( const T&, long );

			template<typename T>
			auto is_equality_comparable_impl( const T& value, int ) -> typename std::is_convertible<decltype(value == value), bool>::type;
		}	// namespace details
		template<typename T>
		struct is_equality_comparable: decltype(details::is_equality_comparable_impl( std::declval<const T&>( ), 1 )) {};

		template<typename T>
		struct is_regular: std::integral_constant < bool, std::is_default_constructible<T>::value &&
			std::is_copy_constructible<T>::value && std::is_move_constructible<T>::value &&
			std::is_copy_assignable<T>::value && std::is_move_assignable<T>::value &&
			is_equality_comparable<T>::value > { };

		template <typename... Args>
		struct max_sizeof;

		//the biggest of one thing is that one thing
		template <typename First>
		struct max_sizeof < First > {
			typedef First type;
			static const size_t value = sizeof( type );
		};

		//the biggest of everything in Args and First
		template <typename First, typename... Args>
		struct max_sizeof < First, Args... > {
			typedef typename max_sizeof<Args...>::type next;
			typedef typename std::conditional<sizeof( First ) >= sizeof( next ), First, next>::type type;
			static const size_t value = sizeof( type );
		};
	
		template<typename BoolType>
		bool are_true( BoolType b1 ) {
			return true == b1;
		}

		template<class BoolType>
		bool are_true( BoolType b1, BoolType b2 ) {
			return are_true( b1 ) && are_true( b2 );
		}

		template<class BoolType, class ...Booleans>
		bool are_true( BoolType b1, BoolType b2, Booleans... others ) {
			return are_true( b1, b2 ) && are_true( others... );
		}

		// 		template<typename... NumericTypes>
		// 		struct is_numeric;
		// 
		// 		template<typename NumericType>
		// 		struct is_numeric < NumericType > {
		// 			static const bool value = std::is_arithmetic<NumericType>::value;
		// 			typedef bool type;
		// 		};
		// 
		// 		template<typename NumericType, typename ...NumericTypes>
		// 		struct is_numeric < NumericType, NumericTypes... > {
		// 			static const bool value = std::is_arithmetic<NumericType>::value && is_numeric<NumericTypes...>::value;
		// 			typedef bool type;
		// 		};

		template<typename... DataTypes>
		struct are_same_types;

		template<typename DataType1, typename DataType2>
		struct are_same_types < DataType1, DataType2 > {
			static const bool value = std::is_same<DataType1, DataType2>::value;
			using type = bool;
		};

		template<typename DataType1, typename DataType2, typename DataType3>
		struct are_same_types < DataType1, DataType2, DataType3 > {
			static const bool value = std::is_same<DataType1, DataType2>::value && std::is_same<DataType1, DataType3>::value;
			using type = bool;
		};

		template<typename DataType1, typename DataType2, typename ...DataTypes>
		struct are_same_types < DataType1, DataType2, DataTypes... > {
			static const bool value = std::is_same<DataType1, DataType2>::value && are_same_types<DataTypes...>::value;
			using type = bool;
		};

		template<typename, typename = void>
		struct has_type_member: std::false_type { };

		template<typename T>
		struct has_type_member<T, void_t<typename T::type>> : std::true_type { };

		template<typename, typename = void>
		struct has_value_type_member: std::false_type { };

		template<typename T>
		struct has_value_type_member<T, void_t<typename T::value_type>> : std::true_type { };

		template<typename, typename = void>
		struct has_iterator_member: std::false_type { };

		template<typename T>
		struct has_iterator_member<T, void_t<typename T::iterator>> : std::true_type { };

		template<typename, typename, typename = void>
		struct has_push_back_member: std::false_type { };

		namespace details {
			template<typename T>
			class has_begin_func {
				typedef char no;
				typedef char yes[2];
				template<class C> static yes& test( char( *)[sizeof( &C::begin )] );
				template<class C> static no& test( ... );
			public:
				enum { value = sizeof( test<T>( 0 ) ) == sizeof( yes& ) };
			};
		}

		template<bool...> struct bool_sequence { };

		template<bool... Bools>
		using bool_and = std::is_same< bool_sequence< Bools...>, bool_sequence< (Bools || true)...> >;

		template<bool... Bools>
		using bool_or = std::integral_constant<bool, !bool_and< !Bools... >::value >;

		template< typename R, bool... Bs > 
		using enable_if_any = std::enable_if< bool_or< Bs... >::value, R >;

		template< typename R, bool... Bs >
		using enable_if_all = std::enable_if< bool_and< Bs... >::value, R >;

		template<typename... DataTypes>
		struct is_one_of;

		template<typename T, typename DataType1>
		struct is_one_of < T, DataType1 > {
			static const bool value = std::is_same<T, DataType1>::value;
			using type = bool;
		};

		template<typename T, typename DataType1, typename DataType2>
		struct is_one_of < T, DataType1, DataType2 > {
			static const bool value = std::is_convertible<typename std::decay<T>::type, DataType1>::value || std::is_same<T, DataType2>::value;
			using type = bool;
		};

		template<typename T, typename DataType1, typename ...DataTypes>
		struct is_one_of < T, DataType1, DataTypes... > {
			static const bool value = std::is_convertible<typename std::decay<T>::type, DataType1>::value || is_one_of<T, DataTypes...>::value;
			using type = bool;
		};

	#define GENERATE_IS_STD_CONTAINER1( ContainerName ) \
		template<typename T, typename = void> struct is_##ContainerName: std::false_type { }; \
		template<typename T> struct is_##ContainerName < T, typename std::enable_if<std::is_convertible<T, std::ContainerName<typename T::value_type> >::value>::type> : std::true_type { };
		
		GENERATE_IS_STD_CONTAINER1( vector );
		GENERATE_IS_STD_CONTAINER1( list );
		GENERATE_IS_STD_CONTAINER1( set );
		GENERATE_IS_STD_CONTAINER1( unordered_set );
		GENERATE_IS_STD_CONTAINER1( deque );

		#undef GENERATE_IS_STD_CONTAINER1

	#define GENERATE_IS_STD_CONTAINER2( ContainerName ) \
		template<typename T, typename = void> struct is_##ContainerName: std::false_type { }; \
		template<typename T> struct is_##ContainerName < T, typename std::enable_if<std::is_convertible<T, std::ContainerName<typename T::key_type, typename T::mapped_type> >::value>::type> : std::true_type { };

		GENERATE_IS_STD_CONTAINER2( map );
		GENERATE_IS_STD_CONTAINER2( unordered_map );

		#undef GENERATE_IS_STD_CONTAINER2


		// Hack, need to figure out a way based on ability at compile time


		template<typename T, typename = void>
		struct is_container: std::false_type { };

		template<typename T>
		struct is_container < T, 
			typename enable_if_any <void,
					is_vector<T>::value,
					is_list<T>::value,
					is_set<T>::value,
					is_deque<T>::value,
					is_unordered_set<T>::value,
					is_map<T>::value,
					is_unordered_map<T>::value
			>::type> : std::true_type { };

		template<typename Numeric, typename = void>
		struct is_numeric: std::false_type { };

		template<typename Numeric>
		struct is_numeric < Numeric, std::enable_if <
			is_one_of < typename std::decay<Numeric>::type,
			char,
			unsigned char,
			int8_t,
			int16_t,
			int32_t,
			int64_t,
			uint8_t,
			uint16_t,
			uint32_t,
			uint64_t,
			float,
			double > ::value >> : std::true_type { };


		template<typename T, typename = void>
		struct is_container_or_array: public std::false_type { };

		template<typename T>
		struct is_container_or_array<T, typename std::enable_if<is_container<T>::value || std::is_array<T>::value>::type> : public std::true_type { };

		template<typename T>
		using is_container_t = typename is_container<T>::type;

		template<typename T>
		using is_array_t = typename std::is_array<T>::type;

		template<typename T>
		using is_container_or_array_t = typename is_container_or_array<T>::type;


		namespace details {
			template<typename T, typename NameGetter>
			class has_member_impl {
				typedef char matched_return_type;
				typedef long unmatched_return_type;

				template<typename C>
				static matched_return_type f( typename NameGetter::template get<C>* );

				template<typename C>
				static unmatched_return_type f( ... );
			public:
				static const bool value = (sizeof( f<T>( 0 ) ) == sizeof( matched_return_type ));
			};
		}	// namespace details

		template<typename T, typename NameGetter>
		struct has_member: std::integral_constant < bool, details::has_member_impl<T, NameGetter>::value > { };
	}	// namespace traits
}	// namespace daw

#define GENERATE_HAS_MEMBER( Member, MemberReturn ) \
	template<typename T, typename = MemberReturn> struct has_##Member: std::false_type {}; \
	template<typename T> struct has_##Member<T, decltype((void)T::Member, 0)> : std::true_type { };
