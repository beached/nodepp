cmake_minimum_required(VERSION 2.8.4)

project(lib_nodepp)

find_package(Boost 1.59.0 COMPONENTS system filesystem regex unit_test_framework REQUIRED)
find_package(OpenSSL REQUIRED)

MESSAGE( WARNING "Current Compiler:" ${CMAKE_CXX_COMPILER_ID} )
IF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC" )
	MESSAGE( WARNING "Using Visual Studio" )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601" )
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

add_subdirectory( ./parse_json )

set(SOURCE_FILES
		base_callback.cpp
		base_callback.h
		base_encoding.cpp
		base_enoding.h
		base_error.cpp
		base_error.h
		base_event_emitter.cpp
		base_event_emitter.h
		base_service_handle.cpp
		base_service_handle.h
		base_stream.h
		base_types.h
		base_url.h
		base_work_queue.cpp
		base_work_queue.h
		base_write_buffer.cpp
		base_write_buffer.h
		daw_semaphore.h
		lib_file.cpp
		lib_file.h
		lib_http_client.h
		lib_http_client.cpp
		lib_http_connection.cpp
		lib_http_connection.h
		lib_http.h
		lib_http_header.cpp
		lib_http_headers.h
		lib_http_request.cpp
		lib_http_request.h
		lib_http_request_parser_impl.cpp
		lib_http_request_parser_impl.h
		lib_http_server.cpp
		lib_http_server.h
		lib_http_server_response.cpp
		lib_http_server_response.h
		lib_http_site.cpp
		lib_http_site.h
		lib_http_statuscodes.cpp
		lib_http_version.cpp
		lib_http_version.h
		lib_http_webservice.h
		lib_net_address.cpp
		lib_net_address.h
		lib_net_dns.cpp
		lib_net_dns.h
		lib_net.h
		lib_net_server.cpp
		lib_net_server.h
		lib_net_socket_stream.cpp
		lib_net_socket_stream.h
		lib_net_socket_boot_socket.h
)

include_directories( ../header_libraries )
include_directories( ./parse_json )
include_directories( ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )
include_directories( ${OPENSSL_INCLUDE_DIR} )


add_library (lib_nodepp STATIC ${SOURCE_FILES})
target_link_libraries( lib_nodepp parse_json )
