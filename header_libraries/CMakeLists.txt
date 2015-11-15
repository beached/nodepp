cmake_minimum_required(VERSION 2.8.0)

project(header_libraries)

MESSAGE( WARNING "Current Compiler:" ${CMAKE_CXX_COMPILER_ID} )
IF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC" )
	MESSAGE( WARNING "Using Visual Studio" )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601" )
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
	MESSAGE( WARNING "Using Clang" )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++ -g -O3 -Weverything -Wno-c++98-compat  -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes")
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
	MESSAGE( WARNING "Using G++" )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O3 -Wall -Wno-deprecated-declarations")
ENDIF( )


find_package(Boost 1.59.0 COMPONENTS system filesystem unit_test_framework REQUIRED)

set(HEADER_PREFIXES
		concurrent_queue
		daw_common_mixins
		daw_range
		daw_string
		daw_traits
		daw_utility
		make_unique
		range_algorithm
		scope_guard
)

enable_testing( )

#add_definitions( -DBOOST_TEST_MAIN )
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK) 
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

foreach( CUR_PREFIX ${HEADER_PREFIXES} )
	add_executable( ${CUR_PREFIX}_test_bin ${CUR_PREFIX}.h ${CUR_PREFIX}_test.cpp header_libraries_test/test_main.cpp )
	target_link_libraries( ${CUR_PREFIX}_test_bin ${Boost_LIBRARIES} )
	add_test( ${CUR_PREFIX}_test, ${CUR_PREFIX}_test_bin )
endforeach( CUR_PREFIX )

