cmake_minimum_required(VERSION 2.8)

project(freeglut_static)

set(LIB_TYPE STATIC)

file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_definitions(-DFREEGLUT_STATIC)

if (MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_library(
	freeglut_static
	${LIB_TYPE} 
	${CMAKE_CURRENT_SOURCE_DIR}/src/freeglutdll.def
	${SOURCE_FILES}
	${HEADER_FILES}
)