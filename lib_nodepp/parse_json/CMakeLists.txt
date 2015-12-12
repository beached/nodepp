cmake_minimum_required(VERSION 2.8.0)

project(parse_json)

find_package(Boost 1.59.0 COMPONENTS system filesystem regex unit_test_framework REQUIRED)

MESSAGE( WARNING "Current Compiler:" ${CMAKE_CXX_COMPILER_ID} )
IF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC" )
	MESSAGE( WARNING "Using Visual Studio" )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601" )
	set( Boost_Libraries )
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
	MESSAGE( WARNING "Using Clang" )
	find_package(Threads REQUIRED)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++ -g -O3 -Weverything -Wno-c++98-compat  -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes")
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
	MESSAGE( WARNING "Using G++" )
	find_package(Threads REQUIRED)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -O3 -Wall -Wno-deprecated-declarations")
ENDIF( )

set(SOURCE_FILES
	daw_json.cpp
	daw_json.h
	daw_json_details.h
	daw_json_interface.h
	daw_json_link.cpp
	daw_json_link.h
	daw_json_parser.cpp
	daw_json_parser.h
)

include_directories( ../../header_libraries )
include_directories( ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )

add_library (parse_json STATIC ${SOURCE_FILES})

#add_executable( parse_json_test_bin test.cpp )
#target_link_libraries( parse_json_test_bin parse_json ${Boost_Libraries} )

enable_testing( )
#add_test( parse_json_test, parse_json_test_bin )

