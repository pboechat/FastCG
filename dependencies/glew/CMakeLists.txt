cmake_minimum_required(VERSION 3.8)

project(glew)

find_package(OpenGL REQUIRED)

list(APPEND HEADERS "${PROJECT_SOURCE_DIR}/include/GL/glew.h" "${PROJECT_SOURCE_DIR}/include/GL/glxew.h" "${PROJECT_SOURCE_DIR}/include/GL/wglew.h")
list(APPEND SOURCES "${PROJECT_SOURCE_DIR}/src/glew.c")

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include 
	CACHE INTERNAL "${PROJECT_NAME}: includes" FORCE)
	
set(${PROJECT_NAME}_LIBRARIES ${PROJECT_NAME} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY}
	CACHE INTERNAL "${PROJECT_NAME}: libraries" FORCE)

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

source_group("include" FILES ${HEADERS})
source_group("src" FILES ${SOURCES})

fastcg_add_dependency_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
add_definitions(-DGLEW_STATIC)
set_target_properties(${PROJECT_NAME} PROPERTIES 
	POSITION_INDEPENDENT_CODE C 
	LINKER_LANGUAGE C
)
