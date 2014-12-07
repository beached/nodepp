#pragma once

#include <cassert>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace daw {
	namespace impl {
		template<typename ResultType, typename... ArgTypes>
		struct make_function_pointer_impl {
			using type = typename std::add_pointer<ResultType( ArgTypes... )>::type;
		};

		template<typename ResultType, typename ClassType, typename... ArgTypes>
		struct make_pointer_to_member_function_impl {
			using type = ResultType( ClassType::* )(ArgTypes...);
		};

		template<typename ResultType, typename ClassType, typename... ArgTypes>
		struct make_pointer_to_volatile_member_function_impl {
			using type = ResultType( ClassType::* )(ArgTypes...) volatile;
		};

		template<typename ResultType, typename ClassType, typename... ArgTypes>
		struct make_pointer_to_const_member_function_impl {
			using type = ResultType( ClassType::* )(ArgTypes...) const;
		};

		template<typename ResultType, typename ClassType, typename... ArgTypes>
		struct make_pointer_to_const_volatile_member_function_impl {
			using type = ResultType( ClassType::* )(ArgTypes...) const volatile;
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

	// For generic types that are functors, delegate to its 'operator()'
	template <typename T>
	struct function_traits: public function_traits < decltype(&T::operator()) > { };

	// for pointers to member function(const version)
	template <typename ClassType, typename ReturnType, typename... Args>
	struct function_traits < ReturnType( ClassType::* )(Args...) const > {
		enum { arity = sizeof...(Args) };
		using type = std::function < ReturnType( Args... ) > ;
		using result_type = ReturnType;
	};

	// for pointers to member function
	template <typename ClassType, typename ReturnType, typename... Args>
	struct function_traits < ReturnType( ClassType::* )(Args...) > {
		enum { arity = sizeof...(Args) };
		using type = std::function < ReturnType( Args... ) > ;
		using result_type = ReturnType;
	};

	// for function pointers
	template <typename ReturnType, typename... Args>
	struct function_traits < ReturnType(*)(Args...) > {
		enum { arity = sizeof...(Args) };
		using type = std::function < ReturnType( Args... ) > ;
		using result_type = ReturnType;
	};

	template <typename L>
	static typename function_traits<L>::type make_function( L l ) {
		return static_cast<typename function_traits<L>::type>(l);
		//return (typename function_traits<L>::type)(l);
	}

	//handles bind & multiple function call operator()'s
	template<typename ReturnType, typename... Args, class T>
	auto make_function( T&& t )	-> std::function < decltype(ReturnType( t( std::declval<Args>( )... ) ))(Args...) > {
		return{ std::forward<T>( t ) };
	}

	//handles explicit overloads
	template<typename ReturnType, typename... Args>
	auto make_function( ReturnType( *p )(Args...) )	-> std::function < ReturnType( Args... ) > {
		return{ p };
	}

	//handles explicit overloads
	template<typename ReturnType, typename... Args, typename ClassType>
	auto make_function( ReturnType( ClassType::*p )(Args...) )	-> std::function < ReturnType( Args... ) > {
		return{ p };
	}

	template<typename T>
	T copy( T const & value ) {
		return value;
	}

	template<typename T>
	std::vector<T> copy_vector( std::vector<T> const & container, size_t num_items ) {
		assert( num_items <= container.size( ) );
		std::vector<T> result( num_items );
		auto first = std::begin( container );
		std::copy( first, first + static_cast<typename std::vector<T>::difference_type>( num_items ), std::begin( result ) );
		return result;
	}

	template<typename T> 
	void copy_vect_and_set( std::vector<T> & source, std::vector<T> & destination, size_t num_items, T const & replacement_value ) {
		using item_size_t = typename std::vector<T>::difference_type;
		assert( num_items < static_cast<size_t>( std::numeric_limits<item_size_t>::max( ) ) );
		auto first = std::begin( source );
		auto last = std::end( source );
		auto max_dist = std::distance( first, last );
		auto items = static_cast<item_size_t>( num_items );
		if( items < max_dist ) {
			last = first + items;
		}

		for( auto it = first; it != last; ++it ) {
			destination.push_back( *it );
			*it = replacement_value;
		}
	}

	template<class T, class U>
	T round_to_nearest( const T& value, const U& rnd_by ) {
		static_assert(std::is_arithmetic<T>::value, "First template parameter must be an arithmetic type");
		static_assert(std::is_floating_point<U>::value, "Second template parameter must be a floating point type");
		const auto rnd = round( static_cast<U>(value) / rnd_by );
		const auto ret = rnd*rnd_by;
		return static_cast<T>(ret);
	}

	template<class T, class U>
	T floor_by( const T& value, const U& rnd_by ) {
		static_assert(std::is_arithmetic<T>::value, "First template parameter must be an arithmetic type");
		static_assert(std::is_floating_point<U>::value, "Second template parameter must be a floating point type");
		const auto rnd = floor( static_cast<U>(value) / rnd_by );
		const auto ret = rnd*rnd_by;
		assert( ret <= value );// , __func__": Error, return value should always be less than or equal to value supplied" );
		return static_cast<T>(ret);
	}

	template<class T, class U>
	T ceil_by( const T& value, const U& rnd_by ) {
		static_assert(std::is_arithmetic<T>::value, "First template parameter must be an arithmetic type");
		static_assert(std::is_floating_point<U>::value, "Second template parameter must be a floating point type");
		const auto rnd = ceil( static_cast<U>(value) / rnd_by );
		const auto ret = rnd*rnd_by;
		assert( ret >= value ); // , __func__": Error, return value should always be greater than or equal to value supplied" );
		return static_cast<T>(ret);
	}

	template<typename T>
	void copy_vect_and_set( std::shared_ptr<std::vector<T>> & source, std::shared_ptr<std::vector<T>> & destination, size_t num_items, T const & replacement_value ) {
		using item_size_t = typename std::vector<T>::difference_type;
		assert( num_items < static_cast<size_t>(std::numeric_limits<item_size_t>::max( )) );
		auto first = std::begin( *source );
		auto last = std::end( *source );
		auto max_dist = std::distance( first, last );
		auto items = static_cast<item_size_t>(num_items);
		if( items < max_dist ) {
			last = first + items;
		}

		for( auto it = first; it != last; ++it ) {
			destination->push_back( *it );
			*it = replacement_value;
		}
	}


}	// namespace daw	
