cmake_minimum_required(VERSION 2.8.0)

project(NodePP)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

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
base_stream.cpp
base_stream.h
base_types.h
lib_http_chunk.cpp
lib_http_chunk.h
lib_http_client_request.cpp
lib_http_client_request.h
lib_http.h
lib_http_header.cpp
lib_http_headers.h
lib_http_server.cpp
lib_http_server.h
lib_http_server_response.cpp
lib_http_server_response.h
lib_http_statuscodes.cpp
lib_net_address.cpp
lib_net_address.h
lib_net.h
lib_net_handle.cpp
lib_net_handle.h
lib_net_server.cpp
lib_net_server.h
lib_net_socket.cpp
lib_net_socket.h
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
utility.h)

add_executable(NodePP ${SOURCE_FILES})

