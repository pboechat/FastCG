cmake_minimum_required(VERSION 3.20)

project(messagebox_x11)

find_package(X11 REQUIRED)

list(APPEND HEADERS "${PROJECT_SOURCE_DIR}/include/messagebox.h")
list(APPEND SOURCES "${PROJECT_SOURCE_DIR}/src/messagebox.cpp")

source_group("include" FILES ${HEADERS})
source_group("src" FILES ${SOURCES})

fastcg_add_dependency_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include ${X11_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES})
