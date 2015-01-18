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
#include <utility>
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

		template<typename... DataTypes>
		struct are_same_types;

		template<typename DataType1, typename DataType2>
		struct are_same_types < DataType1, DataType2 > {
			static bool const value = std::is_same<DataType1, DataType2>::value;
			using type = bool;
		};

		template<typename DataType1, typename DataType2, typename DataType3>
		struct are_same_types < DataType1, DataType2, DataType3 > {
			static bool const value = std::is_same<DataType1, DataType2>::value && std::is_same<DataType1, DataType3>::value;
			using type = bool;
		};

		template<typename DataType1, typename DataType2, typename ...DataTypes>
		struct are_same_types < DataType1, DataType2, DataTypes... > {
			static bool const value = std::is_same<DataType1, DataType2>::value && are_same_types<DataTypes...>::value;
			using type = bool;
		};

		template<bool...> struct bool_sequence { };

		template<bool... Bools>
		using bool_and = std::is_same < bool_sequence< Bools...>, bool_sequence< (Bools || true)...> > ;

		template<bool... Bools>
		using bool_or = std::integral_constant < bool, !bool_and< !Bools... >::value > ;

		template< typename R, bool... Bs >
		using enable_if_any = std::enable_if < bool_or< Bs... >::value, R > ;

		template< typename R, bool... Bs >
		using enable_if_all = std::enable_if < bool_and< Bs... >::value, R > ;

		template<typename T, typename... Types>
		struct is_one_of: std::false_type { };

		template<typename T, typename Type>
		struct is_one_of<T, Type> : std::is_same < T, Type > { };

		template<typename T, typename Type, typename... Types>
		struct is_one_of<T, Type, Types...> : std::integral_constant < bool, std::is_same<T, Type>::value || is_one_of<T, Types...>::value > { };

		namespace details {
			template<typename> struct type_sink { typedef void type; }; // consumes a type, and makes it `void`
			template<typename T> using type_sink_t = typename type_sink<T>::type;
		}

#ifdef _MSC_VER
#define GENERATE_HAS_MEMBER_FUNCTION_TRAIT(MemberName) \
			namespace impl { \
				template<typename T> class has_##MemberName##_member_impl { \
					struct Fallback { int MemberName; }; \
					struct Derived : T, Fallback { }; \
					template<typename U, U> struct Check; \
					typedef char ArrayOfOne[1]; \
					typedef char ArrayOfTwo[2]; \
					template<typename U> static ArrayOfOne & func(Check<int Fallback::*, &U::MemberName> *); \
					template<typename U> static ArrayOfTwo & func(...); \
				  public: \
					typedef has_##MemberName##_member_impl type; \
					enum { value = sizeof(func<Derived>(0)) == 2 }; \
										}; \
						} \
			template<typename T, typename = void> struct has_##MemberName##_member : std::false_type { }; \
			template<typename T> struct has_##MemberName##_member<T, typename std::enable_if<impl::has_##MemberName##_member_impl<T>::value == 1>::type> : std::true_type { }; \
			template<typename T> using has_##MemberName##_member_t = typename has_##MemberName##_member<T>::type;
#else
#define GENERATE_HAS_MEMBER_FUNCTION_TRAIT( MemberName ) \
			template<typename T, typename=void> struct has_##MemberName##_member: std::false_type { }; \
			template<typename T> struct has_##MemberName##_member<T, details::type_sink_t<decltype(std::declval<T>( ).MemberName( ))>>: std::true_type { }; \
			template<typename T> using has_##MemberName##_member_t = typename has_##MemberName##_member<T>::type;
#endif
#define GENERATE_HAS_MEMBER_TYPE_TRAIT( TypeName ) \
			template<typename T, typename=void> struct has_##TypeName##_member: std::false_type { }; \
			template<typename T> struct has_##TypeName##_member<T, details::type_sink_t<typename T::TypeName>>: std::true_type { }; \
			template<typename T> using has_##TypeName##_member_t = typename has_##TypeName##_member<T>::type;

		GENERATE_HAS_MEMBER_FUNCTION_TRAIT( begin );
		GENERATE_HAS_MEMBER_FUNCTION_TRAIT( end );
		GENERATE_HAS_MEMBER_FUNCTION_TRAIT( substr );
		GENERATE_HAS_MEMBER_FUNCTION_TRAIT( push_back );
		GENERATE_HAS_MEMBER_TYPE_TRAIT( type );
		GENERATE_HAS_MEMBER_TYPE_TRAIT( value_type );
		GENERATE_HAS_MEMBER_TYPE_TRAIT( iterator );

		template<typename T, typename = void> struct is_container_like: std::false_type { };
		template<typename T> struct is_container_like<T, typename std::enable_if<has_begin_member<T>::value && has_end_member<T>::value>::type> : std::true_type { };
		template<typename T> using is_container_like_t = typename is_container_like<T>::type;

		template<typename T, typename = void> struct is_string: std::false_type { };
		template<typename T> struct is_string<T, typename std::enable_if<is_container_like_t<T>::value && has_substr_member_t<T>::value>::type> : std::true_type { };
		template<typename T> using is_string_t = typename is_string<T>::type;

		template <typename T>
		using static_not = std::conditional < T::value, std::false_type, std::true_type > ;

		template<typename T, typename = void> struct isnt_string: std::false_type { };
		template<typename T> struct isnt_string<T, typename std::enable_if<!is_string<T>::value>::type> : std::true_type { };
		template<typename T> using isnt_string_t = typename isnt_string<T>::type;

		template<typename T, typename = void> struct is_container_not_string: std::false_type { };
		template<typename T> struct is_container_not_string<T, typename std::enable_if<isnt_string<T>::value && is_container_like<T>::value>::type> : std::true_type { };
		template<typename T> using is_container_not_string_t = typename is_container_not_string<T>::type;

#define GENERATE_IS_STD_CONTAINER1( ContainerName ) \
		template<typename T, typename = void> struct is_##ContainerName: std::false_type { }; \
		template<typename T> struct is_##ContainerName < T, typename std::enable_if<std::is_same<T, std::ContainerName<typename T::value_type> >::value>::type> : std::true_type { };

		GENERATE_IS_STD_CONTAINER1( vector );
		GENERATE_IS_STD_CONTAINER1( list );
		GENERATE_IS_STD_CONTAINER1( set );
		GENERATE_IS_STD_CONTAINER1( unordered_set );
		GENERATE_IS_STD_CONTAINER1( deque );

#undef GENERATE_IS_STD_CONTAINER1

#define GENERATE_IS_STD_CONTAINER2( ContainerName ) \
		template<typename T, typename = void> struct is_##ContainerName: std::false_type { }; \
		template<typename T> struct is_##ContainerName < T, typename std::enable_if<std::is_same<T, std::ContainerName<typename T::key_type, typename T::mapped_type> >::value>::type> : std::true_type { };

		GENERATE_IS_STD_CONTAINER2( map );
		GENERATE_IS_STD_CONTAINER2( unordered_map );

#undef GENERATE_IS_STD_CONTAINER2

		// Hack, need to figure out a way based on ability at compile time

		template<typename Container, typename = void>
		struct is_single_item_container: std::false_type { };

		template<typename Container>
		struct is_single_item_container < Container,
			typename enable_if_any <void,
			is_vector<Container>::value,
			is_list<Container>::value,
			is_set<Container>::value,
			is_deque<Container>::value,
			is_unordered_set<Container>::value
			>::type > : std::true_type { };

		template<typename Container, typename = void>
		struct is_container: std::false_type { };

		template<typename Container>
		struct is_container < Container,
			typename enable_if_any <void,
			is_vector<Container>::value,
			is_list<Container>::value,
			is_set<Container>::value,
			is_deque<Container>::value,
			is_unordered_set<Container>::value,
			is_map<Container>::value,
			is_unordered_map<Container>::value
			>::type > : std::true_type { };

		template<typename Container, typename = void>
		struct is_map_type: std::false_type { };

		template<typename Container>
		struct is_map_type < Container,
			typename enable_if_any <void,
			is_map<Container>::value,
			is_unordered_map<Container>::value
			>::type > : std::true_type { };

		template<typename Numeric, typename = void>
		struct is_numeric: std::false_type { };

		template<typename Numeric>
		struct is_numeric < Numeric, typename std::enable_if <
			is_one_of <typename std::decay<Numeric>::type,
			char,
			unsigned char,
			int,
			unsigned int,
			short,
			unsigned short,
			long,
			unsigned long,
			long long,
			unsigned long long,
			size_t,
			int8_t,
			int16_t,
			int32_t,
			int64_t,
			uint8_t,
			uint16_t,
			uint32_t,
			uint64_t,
			float,
			double > ::value > ::type > : std::true_type { };

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

		//		namespace details {
		//			template<typename T, typename NameGetter>
		//			class has_member_impl {
		//				typedef char matched_return_type;
		//				typedef long unmatched_return_type;
		//
		//				template<typename C>
		//				static matched_return_type f( typename NameGetter::template get<C>* );
		//
		//				template<typename C>
		//				static unmatched_return_type f( ... );
		//			public:
		//				static const bool value = (sizeof( f<T>( 0 ) ) == sizeof( matched_return_type ));
		//			};
		//		}	// namespace details
		//
		//		template<typename T, typename NameGetter>
		//		struct has_member: std::integral_constant < bool, details::has_member_impl<T, NameGetter>::value > { };
	}	// namespace traits
}	// namespace daw

#define GENERATE_HAS_MEMBER( Member, MemberReturn ) \
	template<typename T, typename = MemberReturn> struct has_##Member: std::false_type {}; \
	template<typename T> struct has_##Member<T, decltype((void)T::Member, 0)> : std::true_type { };
