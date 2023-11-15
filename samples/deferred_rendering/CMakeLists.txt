cmake_minimum_required(VERSION 3.20)

project(deferred_rendering)

file(GLOB HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

include_directories(${FastCG_INCLUDE_DIRS})

source_group("include" FILES ${HEADERS})
source_group("src" FILES ${SOURCES})

fastcg_add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})