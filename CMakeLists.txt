cmake_minimum_required(VERSION 2.8.4)
project(NodePP)

find_package(Boost COMPONENTS system filesystem REQUIRED)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Weverything -Wno-c++98-compat -Wno-missing-prototypes -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

set( SOURCE_FILES
    load_library.h
    load_library.cpp
    event_listener.h
    plugin_base.h
    range_algorithm.h
    main.cpp )

#add_definitions( -DBOOST_ALL_NO_LIB )
#add_definitions( -DBOOST_ALL_DYN_LINK )

set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR} )

add_executable(NodePP ${SOURCE_FILES} )



target_link_libraries(NodePP  ${CMAKE_DL_LIBS} ${Boost_LIBRARIES})
