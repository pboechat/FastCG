cmake_minimum_required(VERSION 3.20)

project(FastCG)

include(cmake/fastcg_setup.cmake)
include(cmake/fastcg_targets.cmake)
include(cmake/dependencies.cmake)

add_subdirectory(FastCG)
if (FASTCG_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
