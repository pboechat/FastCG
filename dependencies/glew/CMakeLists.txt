cmake_minimum_required(VERSION 3.20)

project(glew)

if(FASTCG_PLATFORM STREQUAL "Linux")
find_package(X11 REQUIRED)
set(OpenGL_GL_PREFERENCE GLVND)
endif()
find_package(OpenGL REQUIRED)

list(APPEND HEADERS 
	"${PROJECT_SOURCE_DIR}/include/GL/glew.h" 
	"${PROJECT_SOURCE_DIR}/include/GL/glxew.h" 
	"${PROJECT_SOURCE_DIR}/include/GL/wglew.h"
)
list(APPEND SOURCES "${PROJECT_SOURCE_DIR}/src/glew.c")

source_group("include" FILES ${HEADERS})
source_group("src" FILES ${SOURCES})

fastcg_add_dependency_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include ${OPENGL_INCLUDE_DIRS})
target_compile_definitions(${PROJECT_NAME} PUBLIC GLEW_STATIC)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
