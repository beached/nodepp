cmake_minimum_required(VERSION 2.8.0)

project(NodePP)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Weverything -Wno-c++98-compat  -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes")

find_package(Boost 1.55.0 COMPONENTS system filesystem regex REQUIRED)
find_package(Threads REQUIRED)

set(SOURCE_FILES
		base_callback.cpp
		base_callback.h
		base_data.h
		base_encoding.cpp
		base_enoding.h
		base_error.cpp
		base_error.h
		base_event_emitter.cpp
		base_event_emitter.h
		base_service_handle.cpp
		base_service_handle.h
		base_stream.cpp
		base_stream.h
		base_types.h
		lib_http_connection.cpp
		lib_http_connection.h
		lib_http.h
		lib_http_header.cpp
		lib_http_headers.h
		lib_http_request_parser.cpp
		lib_http_request_parser.h
		lib_http_server.cpp
		lib_http_server.h
		lib_http_server_response.cpp
		lib_http_server_response.h
		lib_http_statuscodes.cpp
		lib_http_version.cpp
		lib_http_version.h
		lib_net_address.cpp
		lib_net_address.h
		lib_net_dns.cpp
		lib_net_dns.h
		lib_net.h
		lib_net_server.cpp
		lib_net_server.h
		lib_net_socket.cpp
		lib_net_socket.h
		lib_net_socket_handle.cpp
		lib_net_socket_handle.h
		load_library.cpp
		load_library.h
		load_library_posix.cpp
		load_library_posix.h
		load_plugin.cpp
		load_plugin.h
		main.cpp
		make_unique.h
		plugin_base.h
		range_algorithm.h
		ref_counted_value.h
		string.h
		utility.h
)

add_executable(NodePP ${SOURCE_FILES})

target_link_libraries(NodePP  ${CMAKE_DL_LIBS} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})



