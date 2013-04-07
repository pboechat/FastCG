cmake_minimum_required(VERSION 2.8)

project(glew_static)

set(LIB_TYPE STATIC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_definitions(-DGLEW_STATIC)

if (MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_library(
	glew_static
	${LIB_TYPE} 
	${CMAKE_CURRENT_SOURCE_DIR}/src/glew.c
)