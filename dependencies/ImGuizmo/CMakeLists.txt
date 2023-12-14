cmake_minimum_required(VERSION 3.5)

project(ImGuizmo)

list(APPEND HEADERS 
	${PROJECT_SOURCE_DIR}/include/ImGuizmo.h)
list(APPEND SOURCES ${PROJECT_SOURCE_DIR}/src/ImGuizmo.cpp)

source_group("include" FILES ${HEADERS})
source_group("src" FILES ${SOURCES})

fastcg_add_dependency_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} imgui)