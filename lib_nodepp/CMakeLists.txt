cmake_minimum_required( VERSION 2.8.4 )

project( lib_nodepp )

include( ExternalProject )

find_package( Boost 1.59.0 COMPONENTS system filesystem regex unit_test_framework REQUIRED )
find_package( OpenSSL REQUIRED )

IF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC" )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601" )
	set( Boost_Libraries )
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
	find_package( Threads REQUIRED )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++ -g -O3 -Weverything -Wno-c++98-compat  -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion -Wno-global-constructors" )
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
	find_package( Threads REQUIRED )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -O3 -Wall -Wno-deprecated-declarations" )
ENDIF( )

if( THREADS_HAVE_PTHREAD_ARG )
	set( COMPILER_SPECIFIC_LIBS, "-lpthread" )
endif( )

ExternalProject_Add(
    header_libraries
    GIT_REPOSITORY "https://github.com/beached/header_libraries.git"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/3rdparty/header_libraries"
    GIT_TAG "master"
    PATCH_COMMAND ""
    INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
    TEST_COMMAND ""
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

ExternalProject_Add(
    parse_json
    GIT_REPOSITORY "https://github.com/beached/parse_json.git"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/3rdparty/parse_json"
    GIT_TAG "master"
    PATCH_COMMAND ""
    INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
    TEST_COMMAND ""
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

ExternalProject_Add(
    char_range
    GIT_REPOSITORY "https://github.com/beached/char_range.git"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/3rdparty/char_range"
    GIT_TAG "master"
    PATCH_COMMAND ""
    INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
    TEST_COMMAND ""
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

include_directories( SYSTEM "${CMAKE_BINARY_DIR}/install/include" )
link_directories( "${CMAKE_BINARY_DIR}/install/lib" )

set( HEADER_FILES
		base_callback.h
		base_enoding.h
		base_error.h
		base_event_emitter.h
		base_service_handle.h
		base_stream.h
		base_types.h
		base_work_queue.h
		base_write_buffer.h
		daw_semaphore.h
		lib_file.h
		lib_http_client.h
		lib_http_connection.h
		lib_http.h
		lib_http_headers.h
		lib_http_parser.h
		lib_http_parser_impl.h
		lib_http_request.h
		lib_http_server.h
		lib_http_server_response.h
		lib_http_site.h
		lib_http_url.h
		lib_http_version.h
		lib_http_webservice.h
		lib_net_address.h
		lib_net_dns.h
		lib_net.h
		lib_net_server.h
		lib_net_socket_stream.h
		lib_net_socket_boost_socket.h
		lib_http_client_connection_options.h
 )

set( SOURCE_FILES
		base_callback.cpp
		base_encoding.cpp
		base_error.cpp
		base_event_emitter.cpp
		base_service_handle.cpp
		base_work_queue.cpp
		base_write_buffer.cpp
		lib_file.cpp
		lib_http_client.cpp
		lib_http_connection.cpp
		lib_http_header.cpp
		lib_http_parser.cpp
		lib_http_request.cpp
		lib_http_server.cpp
		lib_http_server_response.cpp
		lib_http_site.cpp
		lib_http_statuscodes.cpp
		lib_http_url.cpp
		lib_http_version.cpp
		lib_net_address.cpp
		lib_net_dns.cpp
		lib_net_server.cpp
		lib_net_socket_stream.cpp
		lib_net_socket_boost_socket.cpp
		lib_http_client_connection_options.cpp
 )

include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
include_directories( SYSTEM ${OPENSSL_INCLUDE_DIR} )

add_library ( lib_nodepp_static STATIC ${HEADER_FILES} ${SOURCE_FILES} )
add_library ( lib_nodepp SHARED ${HEADER_FILES} ${SOURCE_FILES} )
target_link_libraries( lib_nodepp ${Boost_Libraries} ${OPENSSL_LIBRARIES} )
target_link_libraries( lib_nodepp_static ${Boost_Libraries} ${OPENSSL_LIBRARIES} )

add_dependencies( lib_nodepp_static header_libraries char_range parse_json )
add_dependencies( lib_nodepp header_libraries char_range parse_json )

install( TARGETS lib_nodepp DESTINATION lib )
install( TARGETS lib_nodepp_static DESTINATION lib )

foreach( CUR_PREFIX ${HEADER_FILES} )
	install( FILES "${CUR_PREFIX}" DESTINATION include/daw/nodepp )
endforeach( CUR_PREFIX )
