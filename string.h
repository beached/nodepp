#pragma once

#include <boost/lexical_cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/qi_numeric.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <iomanip>
#include <iostream>
#include <locale>
#include <regex>
#include <sstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string>
#include <utility>
#include <vector>
#include <vector>

#ifdef _MSC_VER
#	if _MSC_VER < 1800
#		error Only Visual C++ 2013 and greater is supported
#	elif _MSC_VER == 1800
#		ifndef noexcept
#			define noexcept throw( )
#		endif
#	endif
#endif

namespace daw {
	namespace string {
		namespace details {
			using string_t = std::string;
		}
	}
}

template<typename StringStreamType, typename StringType = daw::string::details::string_t>
void clear( StringStreamType &ss ) {
	ss.str( StringType( ) );
	ss.clear( );
}

namespace daw {
	namespace string {
		namespace details {

			template<typename Arg>
			std::vector<details::string_t> unknowns_to_string( Arg arg ) {
				std::vector<details::string_t> result;
				result.emplace_back( boost::lexical_cast<details::string_t>(arg) );
				return result;
			}

			template<typename Arg, typename... Args>
			std::vector<details::string_t> unknowns_to_string( Arg arg, Args... args ) {
				std::vector<details::string_t> result;
				result.reserve( sizeof...(args)+1 );
				result.emplace_back( boost::lexical_cast<details::string_t>(arg) );
				auto result2 = unknowns_to_string( args... );
				result.insert( std::end( result ), std::begin( result2 ), std::end( result2 ) );
				return result;
			}
		}

		template<typename Delim>
		std::vector<details::string_t> &split( const details::string_t &s, Delim delim, std::vector<details::string_t> &elems ) {
			std::stringstream ss( s );
			details::string_t item;
			while( std::getline( ss, item, delim ) ) {
				elems.push_back( item );
			}
			return elems;
		}

		template<typename Delim>
		std::vector<details::string_t> split( const details::string_t &s, Delim delim ) {
			std::vector<details::string_t> elems;
			split( s, delim, elems );
			return elems;
		}

		template<typename Arg>
		details::string_t to_string( const Arg& arg ) {
			return boost::lexical_cast<details::string_t>(arg);
		}

		namespace {
			template<typename Arg>
			auto string_join( const Arg& arg ) -> decltype(to_string( arg )) {
				return to_string( arg );
			}
		}

		template<typename Arg1, typename Arg2>
		auto string_join( const Arg1& arg1, const Arg2& arg2 ) -> decltype(to_string( arg1 ) + to_string( arg2 )) {
			return to_string( arg1 ) + to_string( arg2 );
		}

		template<typename Arg1, typename Arg2, typename... Args>
		auto string_join( const Arg1& arg1, const Arg2& arg2, const Args & ... args ) -> decltype(string_join( string_join( arg1, arg2 ), string_join( args... ) )) {
			return string_join( string_join( arg1, arg2 ), string_join( args... ) );
		}

		//////////////////////////////////////////////////////////////////////////
		// Summary: takes a format string like "{0} {1} {2}" and in this case 3 parameters
		template<typename Arg, typename... Args>
		details::string_t string_format( details::string_t format, Arg arg, Args... args ) {
			const std::regex reg( R"^(\{(\d+(:\d)*)\})^" );
			static std::stringstream ss;
			clear( ss );
			const auto arguments = details::unknowns_to_string( arg, args... );
			std::smatch sm;
			while( std::regex_search( format, sm, reg ) ) {
				auto& prefix = sm.prefix( ).str( );
				ss << prefix;
				if( ends_with( prefix, "{" ) ) {
					ss << sm[0].str( );
				} else {
					auto delims = split( sm[1].str( ), ':' );
					if( 1 >= delims.size( ) ) {
						ss << arguments[boost::lexical_cast<size_t>(sm[1].str( ))];
					} else if( 2 == delims.size( ) ) {
						// Assumes the argument at pos is a double.  If not, will crash
						size_t pos = boost::lexical_cast<size_t>(delims[0]);
						size_t precision = boost::lexical_cast<size_t>(delims[1]);
						ss << std::fixed << std::setprecision( precision ) << boost::lexical_cast<double>(arguments[pos]);
					} else {
						throw std::out_of_range( string_format( "Unknown string format.  Too many colons(", delims.size( ), "): ", sm[1].str( ) ) );
					}
				}
				format = sm.suffix( ).str( );
			}
			ss << format;
			return ss.str( );
		}

		template < typename StringType >
		struct ends_with_t {
			ends_with_t( ) noexcept{ };
			~ends_with_t( ) = default;
			ends_with_t( const ends_with_t& ) noexcept{ };
			ends_with_t( ends_with_t&& ) noexcept{ }
			ends_with_t& operator=(ends_with_t) const noexcept{ return *this; }
			bool operator==(const ends_with_t&) const noexcept{ return true; }
			bool operator<(const ends_with_t&) const noexcept{ return false; }

				bool operator()( const StringType& src, const char ending ) const noexcept{
				return 0 < src.size( ) && ending == src[src.size( ) - 1];
			}

				bool operator()( const StringType& src, const StringType& ending ) const noexcept{
				auto pos = src.find_last_of( ending );
				return details::string_t::npos != pos && pos == src.size( ) - 1;
			}
		};

		template<typename StringType, typename Ending>
		bool ends_with( const StringType& src, const Ending& ending ) {
			const static auto func = ends_with_t<StringType>( );
			return func( src, ending );
		}

		template<typename StringType>
		StringType trim_right_copy( const StringType& s, const StringType& delimiters = StringType( " \f\n\r\t\v\0" ) ) {
			if( 0 == s.size( ) ) {
				return s;
			}
			return s.substr( 0, s.find_last_not_of( delimiters ) + 1 );
		}

		template<typename StringType>
		StringType trim_left_copy( const StringType& s, const StringType& delimiters = StringType( " \f\n\r\t\v\0" ) ) {
			if( 0 == s.size( ) ) {
				return s;
			}
			return s.substr( s.find_first_not_of( delimiters ) );
		}

		template<class StringElementType, class StringType>
		bool in( const StringElementType& val, const StringType& values ) {
			return StringType::npos != values.find( val );
		}

		template<typename StringType>
		void trim_nocopy( StringType& str, const StringType& chars = " \f\n\r\t\v\0" ) {
			const auto start = str.find_first_not_of( chars );
			if( StringType::npos == start ) {
				str.clear( );
				return;
			}
			const auto end = str.find_last_not_of( chars );
			const auto len = end - start + 1;
			str = str.substr( start, len );
		}

		template<typename StringType>
		StringType trim_copy( StringType s, const StringType& delimiters = " \f\n\r\t\v\0" ) {
			trim_nocopy( s, delimiters );
			return s;
		}

		template<typename StringType>
		bool conertTo( const StringType& from, int& to ) {
			auto it = from.begin( );
			using namespace boost::spirit;
			if( !qi::parse( it, from.end( ), qi::int_, to ) ) {
				return false;
			}
			return from.end( ) == it;
		}

		template<typename StringType>
		bool convertTo( const StringType& from, int64_t& to ) {
			auto it = from.begin( );
			using namespace boost::spirit;
			if( !qi::parse( it, from.end( ), qi::long_long, to ) ) {
				return false;
			}
			return from.end( ) == it;
		}

		template<typename StringType>
		bool convertTo( const StringType& from, float& to ) {
			auto it = from.begin( );
			using namespace boost::spirit;
			if( !qi::parse( it, from.end( ), qi::float_, to ) ) {
				return false;
			}
			return from.end( ) == it;
		}

		template<typename StringType>
		bool convertTo( const StringType& from, double& to ) {
			auto it = from.begin( );
			using namespace boost::spirit;
			if( !qi::parse( it, from.end( ), qi::double_, to ) ) {
				return false;
			}
			return from.end( ) == it;
		}

		template<class T, class U>
		std::string convertToString( const std::pair<T, U>& from ) {
			return string_join( "{", to_string( from.first ), ", ", to_string( from.second ), " }" );
		}

		template<class ValueType>
		void convertToString( const ValueType& from, details::string_t& to ) {
			to = to_string( from );
		}

		template<class ValueType>
		void convertToString( const ValueType& from, details::string_t& to, const details::string_t& locale_str ) {
#pragma message( "Use non-locale version" )
			static std::stringstream ss;
			clear( ss );
			ss << from;
			ss.imbue( std::locale( locale_str ) );
			ss >> to;
		}



	}	// namespace string
}	// namespace daw
