cmake_minimum_required(VERSION 3.5)

project(ImGuiFileDialog)

list(APPEND HEADERS 
	${PROJECT_SOURCE_DIR}/include/ImGuiFileDialog.h
	${PROJECT_SOURCE_DIR}/include/ImGuiFileDialogConfig.h)
list(APPEND SOURCES ${PROJECT_SOURCE_DIR}/src/ImGuiFileDialog.cpp)

source_group("include" FILES ${HEADERS})
source_group("src" FILES ${SOURCES})

fastcg_add_dependency_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(
	${PROJECT_NAME} 
	PRIVATE 
		USE_STD_FILESYSTEM
)
target_link_libraries(${PROJECT_NAME} imgui)