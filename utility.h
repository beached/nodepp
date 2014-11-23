#pragma once

#include <string>
#include <type_traits>
#include <vector>


namespace daw {
	namespace impl {
		template<typename ResultType, typename... ArgTypes>
		struct make_function_pointer_impl {
			using type = typename std::add_pointer<ResultType(ArgTypes...)>::type;
		};

		template<typename ResultType, typename ClassType, typename... ArgTypes>
		struct make_pointer_to_member_function_impl {
			using type = ResultType(ClassType::*)(ArgTypes...);
		};

		template<typename ResultType, typename ClassType, typename... ArgTypes>
		struct make_pointer_to_volatile_member_function_impl {
			using type = ResultType(ClassType::*)(ArgTypes...) volatile;
		};

		template<typename ResultType, typename ClassType, typename... ArgTypes>
		struct make_pointer_to_const_member_function_impl {
			using type = ResultType(ClassType::*)(ArgTypes...) const;
		};

		template<typename ResultType, typename ClassType, typename... ArgTypes>
		struct make_pointer_to_const_volatile_member_function_impl {
			using type = ResultType(ClassType::*)(ArgTypes...) const volatile;
		};
	}	// namespace impl

	template<typename ResultType, typename... ArgTypes>
	using function_pointer_t = typename impl::make_function_pointer_impl<ResultType, ArgTypes...>::type;

	template<typename ResultType, typename ClassType, typename... ArgTypes>
	using pointer_to_member_function_t = typename impl::make_pointer_to_member_function_impl<ResultType, ClassType, ArgTypes...>::type;

	template<typename ResultType, typename ClassType, typename... ArgTypes>
	using pointer_to_volatile_member_function_t = typename impl::make_pointer_to_volatile_member_function_impl<ResultType, ClassType, ArgTypes...>::type;

	template<typename ResultType, typename ClassType, typename... ArgTypes>
	using pointer_to_const_member_function_t = typename impl::make_pointer_to_const_member_function_impl<ResultType, ClassType, ArgTypes...>::type;

	template<typename ResultType, typename ClassType, typename... ArgTypes>
	using pointer_to_const_volatile_member_function_t = typename impl::make_pointer_to_const_volatile_member_function_impl<ResultType, ClassType, ArgTypes...>::type;
	



}	// namespace daw