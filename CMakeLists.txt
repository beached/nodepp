cmake_minimum_required(VERSION 2.8.4)
project(NodePP)

find_package(Boost COMPONENTS filesystem REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set( SOURCE_FILES
    load_library.h
    load_library.cpp
    event_listener.h
    plugin_base.h
    range_algorithm.h
    main.cpp )

set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR} )

add_executable(NodePP ${SOURCE_FILES} ${Boost_LIBRARIES})



target_link_libraries(NodePP  ${CMAKE_DL_LIBS} )
