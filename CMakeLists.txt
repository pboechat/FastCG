cmake_minimum_required(VERSION 2.8)

# option(TESTS "Build Tests" ON)

project(FastCG)

if(!WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(src)
add_subdirectory(dependencies)

if (TESTS)
  add_subdirectory(tests)
endif()

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})