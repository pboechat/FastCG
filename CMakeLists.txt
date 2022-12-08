cmake_minimum_required(VERSION 3.8)

project(FastCG)

include(cmake/fastcg_setup.cmake)
include(cmake/fastcg_targets.cmake)

add_subdirectory(dependencies)
add_subdirectory(FastCG)
if (FASTCG_BUILD_SAMPLES)
    add_subdirectory(samples)
endif()