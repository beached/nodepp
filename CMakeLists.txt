cmake_minimum_required(VERSION 2.8.4)
project(NodePP)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    load_library.h
    load_library_test.cpp
    testlib.cpp)
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR} )

add_library( testlib MODULE testlib.cpp)

add_executable(NodePP load_library.h load_library_test.cpp)

target_link_libraries(NodePP  ${CMAKE_DL_LIBS})
