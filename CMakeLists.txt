cmake_minimum_required(VERSION 2.8.4)
project(NodePP)

find_package(Boost COMPONENTS system filesystem REQUIRED)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Weverything -Wno-c++98-compat -Wno-missing-prototypes -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

set( SOURCE_FILES
    event_listener.h
    lib_address.h
    lib_error.h
    lib_http.h
    lib_http.cpp
    lib_net.h
    lib_net_handle.h
    lib_net_server.h
    lib_net_socket.h
    lib_net_socket.h
    lib_types.h
    load_library.h
    load_library.cpp
    load_library_posix.h
    load_library_posix.cpp
    main.cpp
    plugin_base.h
    range_algorithm.h
    ref_counted_value.h
    utility.h)

#add_definitions( -DBOOST_ALL_NO_LIB )
#add_definitions( -DBOOST_ALL_DYN_LINK )

set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR} )

add_executable(NodePP ${SOURCE_FILES} )



target_link_libraries(NodePP  ${CMAKE_DL_LIBS} ${Boost_LIBRARIES})
