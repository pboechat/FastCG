cmake_minimum_required(VERSION 3.8)

project(messagebox_x11)

find_package(X11 REQUIRED)

list(APPEND HEADERS "${PROJECT_SOURCE_DIR}/include/messagebox.h")
list(APPEND SOURCES "${PROJECT_SOURCE_DIR}/src/messagebox.cpp")

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include ${X11_INCLUDE_DIR}
	CACHE INTERNAL "${PROJECT_NAME}: includes" FORCE)
	
set(${PROJECT_NAME}_LIBRARIES ${PROJECT_NAME} ${X11_LIBRARIES}
	CACHE INTERNAL "${PROJECT_NAME}: libraries" FORCE)

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

source_group("include" FILES ${HEADERS})
source_group("src" FILES ${SOURCES})

fastcg_add_dependency_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES 
	POSITION_INDEPENDENT_CODE CXX
	LINKER_LANGUAGE CXX
)
