cmake_minimum_required(VERSION 2.8.4)

project(NodePP)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.59.0 REQUIRED COMPONENTS system filesystem regex unit_test_framework program_options)
set(Boost_Libs ${BOOST_Libraries} -lboost_program_options -lboost_system -lboost_regex -lboost_filesystem)

find_package(OpenSSL REQUIRED)

MESSAGE( WARNING "Current Compiler:" ${CMAKE_CXX_COMPILER_ID} )
IF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC" )
	MESSAGE( WARNING "Using Visual Studio" )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601" )
	set( Boost_Libs )
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
	MESSAGE( WARNING "Using Clang" )
	find_package(Threads REQUIRED)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++ -g -O3 -Weverything -Wno-c++98-compat  -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes")
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
	MESSAGE( WARNING "Using G++" )
	find_package(Threads REQUIRED)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -O3 -Wall -Wno-deprecated-declarations")
ENDIF( )

if(THREADS_HAVE_PTHREAD_ARG)
	set(COMPILER_SPECIFIC_LIBS, "-lpthread" )
endif( )

enable_testing( )

add_subdirectory( ./lib_nodepp )
add_subdirectory( ./header_libraries )
add_subdirectory( ./load_library )

include_directories( ./lib_nodepp )
include_directories( ./lib_nodepp/parse_json )
include_directories( ./header_libraries )
include_directories( ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )
include_directories( ${OPENSSL_INCLUDE_DIR} )

add_executable(example_net_server example_net_server.cpp )
target_link_libraries(example_net_server lib_nodepp ${CMAKE_DL_LIBS} ${Boost_Libs} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS})

add_executable(example_net_client example_net_client.cpp )
target_link_libraries(example_net_client lib_nodepp ${Boost_Libs} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS})

add_executable(example_web_service example_web_service.cpp )
target_link_libraries(example_web_service lib_nodepp parse_json ${CMAKE_DL_LIBS} ${Boost_Libs} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS})

add_executable(example_web_client example_web_client.cpp )
target_link_libraries(example_web_client lib_nodepp parse_json ${CMAKE_DL_LIBS} ${Boost_Libs} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS})

add_executable(example_event_emitter example_event_emitter.cpp )
target_link_libraries(example_event_emitter lib_nodepp ${CMAKE_DL_LIBS} ${Boost_Libs} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS})

add_executable(example_parse_json example_parse_json.cpp )
target_link_libraries(example_parse_json parse_json ${CMAKE_DL_LIBS} ${Boost_Libs} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS})
