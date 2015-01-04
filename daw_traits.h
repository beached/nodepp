#pragma once

#include <type_traits>
#include <utility>

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

		template<class BoolType>
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

		template<typename... NumericTypes>
		struct is_numeric;

		template<typename NumericType>
		struct is_numeric < NumericType > {
			static const bool value = std::is_arithmetic<NumericType>::value;
			typedef bool type;
		};

		template<typename NumericType, typename ...NumericTypes>
		struct is_numeric < NumericType, NumericTypes... > {
			static const bool value = std::is_arithmetic<NumericType>::value && is_numeric<NumericTypes...>::value;
			typedef bool type;
		};

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
			static const bool value = std::is_same<DataType1, DataType2>::value && are_same_type<DataTypes...>::value;
			using type = bool;
		};


		template<typename, typename = void>
		struct has_type_member: std::false_type { };

		template<typename T>
		struct has_type_member<T, void_t<typename T::type>> : std::true_type { };

		template<typename, typename = void>
		struct has_iterator_member: std::false_type { };

		template<typename T>
		struct has_iterator_member<T, void_t<typename T::iterator>> : std::true_type { };


		template<typename, typename, typename = void>
		struct has_push_back_member: std::false_type { };
	

	}	// namespace traits
}	// namespace daw

