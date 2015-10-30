cmake_minimum_required(VERSION 2.8.0)

project(example_load_library)

MESSAGE( WARNING "Current Compiler:" ${CMAKE_CXX_COMPILER_ID} )
IF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC" )
	MESSAGE( WARNING "Using Visual Studio" )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601" )
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
	MESSAGE( WARNING "Using Clang" )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++ -g -O3 -Weverything -Wno-c++98-compat  -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes")
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
	MESSAGE( WARNING "Using G++" )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O3 -Wall")
ENDIF( )

find_package(Boost 1.55.0 COMPONENTS system filesystem unit_test_framework REQUIRED)

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

include_directories( ../header_libraries )

add_library (load_library STATIC ${SOURCE_FILES})

add_executable(example_load_library example_load_library.cpp )
target_link_libraries(example_load_library load_library ${CMAKE_DL_LIBS} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS})

#enable_testing( )
#add_test( parse_json_test, parse_json_test_bin )

