cmake_minimum_required(VERSION 2.8.4)
project(NodePP)


set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.59.0 REQUIRED COMPONENTS system filesystem regex unit_test_framework program_options iostreams)
set(Boost_Libs ${BOOST_Libraries} -lboost_system -lboost_regex -lboost_filesystem -lboost_program_options -lboost_iostreams )

find_package(OpenSSL REQUIRED)

IF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC" )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601 /MP" )
	set( Boost_Libs )
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
	find_package(Threads REQUIRED)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++ -g -O3 -Weverything -Wno-c++98-compat  -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion -Wno-global-constructors")
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
	find_package(Threads REQUIRED)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -O3 -Wall -Wno-deprecated-declarations")
ENDIF( )

if(THREADS_HAVE_PTHREAD_ARG)
	set(COMPILER_SPECIFIC_LIBS, "-lpthread" )
endif( )

include(ExternalProject)
ExternalProject_Add( 
	header_libraries
	GIT_REPOSITORY "https://github.com/beached/header_libraries.git"
	SOURCE_DIR "${CMAKE_BINARY_DIR}/3rdparty/header_libraries"
	GIT_TAG "master"
	PATCH_COMMAND ""
	INSTALL_COMMAND ""
	TEST_COMMAND ""
)

set( header_libraries_include "${CMAKE_BINARY_DIR}/3rdparty/header_libraries" )
set_property( GLOBAL PROPERTY header_libraries_include ${header_libraries_include} )
include_directories( SYSTEM ${header_libraries_include} )

add_subdirectory( ./lib_nodepp )
add_dependencies( lib_nodepp header_libraries )
add_subdirectory( ./load_library )
add_dependencies( load_library header_libraries )
add_subdirectory( ./parse_template )
add_dependencies( parse_template header_libraries )
add_subdirectory( ./char_range ${CMAKE_CURRENT_BINARY_DIR}/char_range )

include_directories( ./lib_nodepp )
include_directories( ./lib_nodepp/parse_json )
include_directories( ./parse_template )
include_directories( ./char_range )
include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )
include_directories( SYSTEM ${OPENSSL_INCLUDE_DIR} )

add_executable(example_net_server example_net_server.cpp )
target_link_libraries(example_net_server char_range lib_nodepp ${CMAKE_DL_LIBS} ${Boost_Libs} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS})
add_test( example_net_server_test, example_net_server )

add_executable(example_net_client example_net_client.cpp )
target_link_libraries(example_net_client char_range lib_nodepp ${Boost_Libs} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS})
add_test( example_net_client_test, example_net_client )

add_executable(example_web_service example_web_service.cpp )
target_link_libraries(example_web_service char_range lib_nodepp parse_json ${CMAKE_DL_LIBS} ${Boost_Libs} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS})
add_test( example_web_service_test, example_web_service )

add_executable(example_web_client example_web_client.cpp )
target_link_libraries(example_web_client lib_nodepp parse_json char_range ${CMAKE_DL_LIBS} ${Boost_Libs} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS})
add_test( example_web_client_test, example_web_client )

add_executable(example_event_emitter example_event_emitter.cpp )
target_link_libraries(example_event_emitter char_range lib_nodepp ${CMAKE_DL_LIBS} ${Boost_Libs} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS})
add_test( example_event_emitter_test, example_event_emitter )

add_executable(example_parse_json example_parse_json.cpp )
target_link_libraries(example_parse_json parse_json char_range ${CMAKE_DL_LIBS} ${Boost_Libs} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS})
add_test( example_parse_json_test, example_parse_json )

add_executable(example_parse_template example_parse_template.cpp )
target_link_libraries(example_parse_template parse_template char_range ${CMAKE_DL_LIBS} ${Boost_Libs} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS})
add_test( example_parse_template_test, example_parse_template )

