cmake_minimum_required(VERSION 3.20)

project(tinyobj_loader_c)

list(APPEND HEADERS "${PROJECT_SOURCE_DIR}/include/tinyobj_loader_c.h")

source_group("include" FILES ${HEADERS})

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${PROJECT_SOURCE_DIR}/include)