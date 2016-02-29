cmake_minimum_required(VERSION 2.8.0)

project(parse_template)

find_package(Boost 1.59.0 COMPONENTS system filesystem regex unit_test_framework REQUIRED)
set(Boost_Libs ${BOOST_Libraries} -lboost_system -lboost_regex -lboost_filesystem -lboost_program_options -lboost_unit_test_framework )

IF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC" )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601" )
	set( Boost_Libs )
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
	find_package(Threads REQUIRED)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++ -g -O3 -Weverything -Wno-c++98-compat  -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion")
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
	find_package(Threads REQUIRED)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -O3 -Wall -Wno-deprecated-declarations")
ENDIF( )

set(SOURCE_FILES
	daw_template.h
	daw_parse_template.h
	daw_parse_template.cpp
)

include_directories( ../lib_nodepp )
include_directories( ../header_libraries )
include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )

add_library (parse_template SHARED ${SOURCE_FILES})

enable_testing( )

#add_executable( parse_template_test_bin parse_template_test.cpp )
#target_link_libraries( parse_template_test_bin parse_template ${Boost_Libs} )

#add_test( parse_template_test, parse_template_test_bin )

