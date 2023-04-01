cmake_minimum_required(VERSION 3.8)

project(FastCG)

set(FASTCG_GRAPHICS_SYSTEM "OpenGL" CACHE STRING "" FORCE)
set(FASTCG_USE_TEXT_SHADERS TRUE CACHE BOOL "" FORCE)

include(cmake/fastcg_setup.cmake)
include(cmake/fastcg_targets.cmake)

add_subdirectory(dependencies)
add_subdirectory(FastCG)
if (FASTCG_BUILD_SAMPLES)
    add_subdirectory(samples)
endif()