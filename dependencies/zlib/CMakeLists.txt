cmake_minimum_required(VERSION 3.20)

project(zlib)

list(APPEND HEADERS "${PROJECT_SOURCE_DIR}/include/zstd.h")
list(APPEND SOURCES "${PROJECT_SOURCE_DIR}/src/zstd.c")

source_group("include" FILES ${HEADERS})
source_group("src" FILES ${SOURCES})

fastcg_add_dependency_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)