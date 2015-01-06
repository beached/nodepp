cmake_minimum_required(VERSION 2.8.0)

project(NodePP)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O3 -Weverything -Wno-c++98-compat  -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -v")

find_package(Boost 1.55.0 COMPONENTS system filesystem regex REQUIRED)
find_package(Threads REQUIRED)

set(SOURCE_FILES
		base_callback.cpp
		base_callback.h
		base_encoding.cpp
		base_enoding.h
		base_error.cpp
		base_error.h
		base_event_emitter.cpp
		base_event_emitter.h
		base_json.cpp
		base_json.h
		base_service_handle.cpp
		base_service_handle.h
		base_stream.h
		base_types.h
		base_url.h
		base_work_queue.cpp
		base_work_queue.h
		base_write_buffer.cpp
		base_write_buffer.h
		concurrent_queue.h
		daw_optional.h
		daw_traits.h
		lib_file.cpp
		lib_file.h
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
		lib_net_address.cpp
		lib_net_address.h
		lib_net_dns.cpp
		lib_net_dns.h
		lib_net.h
		lib_net_server.cpp
		lib_net_server.h
		lib_net_socket_stream.cpp
		lib_net_socket_stream.h
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
		scope_guard.h
		semaphore.h
		string.h
		utility.h
)

add_executable(NodePP ${SOURCE_FILES})

target_link_libraries(NodePP  ${CMAKE_DL_LIBS} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})


