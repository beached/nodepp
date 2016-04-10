cmake_minimum_required(VERSION 2.8.0)

project(example_load_library)

find_package(Boost 1.59.0 COMPONENTS system filesystem regex unit_test_framework REQUIRED)
set(Boost_LIBS -lboost_system -lboost_filesystem -lboost_regex -lboost_unit_test_framework)

IF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC" )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601" )
	set( Boost_LIBS )
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
	find_package(Threads REQUIRED)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++ -g -O3 -Weverything -Wno-c++98-compat  -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion")
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
	find_package(Threads REQUIRED)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -O3 -Wall -Wno-deprecated-declarations")
ENDIF( )

set(SOURCE_FILES
	load_library.cpp
	load_library.h
	load_library_posix.cpp
	load_library_posix.h
	load_library_test.cpp
	load_library_windows.cpp
	load_library_windows.h
	load_plugin.cpp
	load_plugin.h
	plugin_base.h
	ref_counted_value.h
)

include_directories( "${CMAKE_BINARY_DIR}/3rdparty/header_libraries" )

include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )

add_library (load_library STATIC ${SOURCE_FILES})

add_executable(example_load_library example_load_library.cpp )
target_link_libraries(example_load_library load_library ${CMAKE_DL_LIBS} ${Boost_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS})

#enable_testing( )
#add_test( parse_json_test, parse_json_test_bin )

