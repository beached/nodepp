cmake_minimum_required( VERSION 2.8.4 )
project( nodepp_examples_prj )

include( ExternalProject )

set( Boost_USE_STATIC_LIBS OFF ) 
set( Boost_USE_MULTITHREADED ON )  
set( Boost_USE_STATIC_RUNTIME OFF ) 
find_package( Boost 1.59.0 REQUIRED COMPONENTS system filesystem regex unit_test_framework program_options iostreams )
find_package( OpenSSL REQUIRED )
#set( Library_Dependencies ${BOOST_Libraries} -lboost_system -lboost_regex -lboost_filesystem -lboost_program_options -lboost_iostreams ${OPENSSL_LIBRARIES} )
set( Library_Dependencies ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES} )

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL 'MSVC' )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601 /MP" )
	set( Library_Dependencies )
elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
	find_package( Threads REQUIRED )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++ -g -O3 -Weverything -Wno-c++98-compat  -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion -Wno-global-constructors" )
elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
	find_package( Threads REQUIRED )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -O3 -Wall -Wno-deprecated-declarations" )
endif( )

if( THREADS_HAVE_PTHREAD_ARG )
	set( COMPILER_SPECIFIC_LIBS, "-lpthread" )
endif( )

externalproject_add( 
	header_libraries_prj
	GIT_REPOSITORY "https://github.com/beached/header_libraries.git"
	SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/header_libraries"
	GIT_TAG "master"
	PATCH_COMMAND ""
	INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
	TEST_COMMAND ""
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

externalproject_add( 
	char_range_prj
	GIT_REPOSITORY "https://github.com/beached/char_range.git"
	SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/char_range"
	GIT_TAG "master"
	PATCH_COMMAND ""
	INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
	TEST_COMMAND ""
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

externalproject_add( 
	parse_json_prj
	GIT_REPOSITORY "https://github.com/beached/parse_json.git"
	SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/parse_json"
	GIT_TAG "master"
	PATCH_COMMAND ""
	INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
	TEST_COMMAND ""
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

externalproject_add( 
	parse_template_prj
	GIT_REPOSITORY "https://github.com/beached/parse_template.git"
	SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/parse_template"
	GIT_TAG "master"
	PATCH_COMMAND ""
	INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
	TEST_COMMAND ""
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

externalproject_add( 
	lib_nodepp_prj
	GIT_REPOSITORY "https://github.com/beached/lib_nodepp.git"
	SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/lib_nodepp"
	GIT_TAG "master"
	PATCH_COMMAND ""
	INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
	TEST_COMMAND ""
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

include_directories( SYSTEM "${CMAKE_BINARY_DIR}/install/include" )
link_directories( "${CMAKE_BINARY_DIR}/install/lib" )

include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )
include_directories( SYSTEM ${OPENSSL_INCLUDE_DIR} )

set( NODEPP_DEPS header_libraries_prj char_range_prj parse_json_prj lib_nodepp_prj )
set( NODEPP_LIBS nodepp char_range parse_json ${Library_Dependencies} )

add_executable( example_net_server example_net_server.cpp )
target_link_libraries( example_net_server ${NODEPP_LIBS} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS} )
add_dependencies( example_net_server ${NODEPP_DEPS} )
add_test( example_net_server_test, example_net_server )

add_executable( example_net_client example_net_client.cpp )
target_link_libraries( example_net_client ${NODEPP_LIBS} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS} )
add_dependencies( example_net_client ${NODEPP_DEPS} )
add_test( example_net_client_test, example_net_client )

add_executable( example_web_service example_web_service.cpp )
target_link_libraries( example_web_service ${NODEPP_LIBS} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS} )
add_dependencies( example_web_service ${NODEPP_DEPS} )
add_test( example_web_service_test, example_web_service )

add_executable( example_web_client example_web_client.cpp )
target_link_libraries( example_web_client ${NODEPP_LIBS} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS} )
add_dependencies( example_web_client ${NODEPP_DEPS} )
add_test( example_web_client_test, example_web_client )

add_executable( example_event_emitter example_event_emitter.cpp )
target_link_libraries( example_event_emitter ${NODEPP_LIBS} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS} )
add_dependencies( example_event_emitter ${NODEPP_DEPS} )
add_test( example_event_emitter_test, example_event_emitter )

add_executable( example_parse_json example_parse_json.cpp )
target_link_libraries( example_parse_json parse_json char_range ${Library_Dependencies} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS} )
add_dependencies( example_parse_json header_libraries_prj parse_json_prj char_range_prj )
add_test( example_parse_json_test, example_parse_json )

add_executable( example_parse_template example_parse_template.cpp )
target_link_libraries( example_parse_template char_range parse_template ${Library_Dependencies} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${COMPILER_SPECIFIC_LIBS} )
add_dependencies( example_parse_template header_libraries_prj char_range_prj parse_template_prj )
add_test( example_parse_template_test, example_parse_template )

