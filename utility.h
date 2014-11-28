#pragma once

#include <functional>
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


// 	namespace details {
// 		template<typename Func>
// 		std::false_type is_function( Func );
// 
// 		template<typename Func>
// 		auto is_function( Func ) -> typename std::is_convertible< Func, std::function< void( ) > >::type;
// 	}	// namespace details
// 	template<typename Func>
// 	struct is_function: decltype(std::function( std::declval<Func( )> )) {};

	namespace impl {
		template<typename T>
		class EqualToImpl {
			T m_value;
		public:
			EqualToImpl( ) = delete;
			~EqualToImpl( ) = default;
			EqualToImpl( EqualToImpl const & ) = default;
			EqualToImpl& operator=(EqualToImpl const &) = default;
			EqualToImpl( EqualToImpl && other ) : m_value( std::move( other.m_value ) ) { }
			EqualToImpl& operator=(EqualToImpl && rhs) {
				if( this != &rhs ) {
					m_value = std::move( rhs.m_value );
				}
				return *this;
			}
			EqualToImpl( T value ) :m_value( value ) { }
			bool operator()( T const & value ) {
				return m_value == value;
			}
		};	// class EqualToImpl
	
	}	// namespace impl
	template<typename T>
	impl::EqualToImpl<T> equal_to( T value ) {
		return impl::EqualToImpl<T>( std::move( value ) );
	}

	template<typename T>
	class equal_to_last {
		T* m_value;
	public:
		equal_to_last( ) : m_value( nullptr ) { }
		~equal_to_last( ) = default;
		equal_to_last& operator=(equal_to_last const &) = default;
		equal_to_last( equal_to_last && other ) : m_value( std::move( other.m_value ) ) { }
		equal_to_last& operator=(equal_to_last && rhs) {
			if( this != &rhs ) {
				m_value = std::move( rhs.m_value );
			}
			return *this;
		}
		bool operator()( T const & value ) {
			bool result = false;
			if( m_value ) {
				result = *m_value == value;
			}
			m_value = const_cast<T*>(&value);
			return result;
		}
	};	// class equal_to_last

	namespace impl {
	template<typename Function>
		class NotImpl {
			Function m_function;
		public:
			NotImpl( Function func ) : m_function( func ) { }
			~NotImpl( ) = default;
			NotImpl& operator=(NotImpl const &) = default;
			NotImpl( NotImpl && other ) : m_function( std::move( other.m_function ) ) { }
			NotImpl& operator=(NotImpl && rhs) {
				if( this != &rhs ) {
					m_function = std::move( rhs.m_function );
				}
				return *this;
			}
			
			template<typename...Args>		
			bool operator()( Args&&... args ) {
				return !m_function( std::forward<Args>( args )... );
			}
		};	// class NotImpl
}	// namespace impl

	template<typename Function>
	impl::NotImpl<Function> Not( Function func ) {
		return impl::NotImpl<Function>( func );
	}

}	// namespace daw	

