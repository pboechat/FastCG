cmake_minimum_required(VERSION 3.20)

project(imgui)

list(APPEND HEADERS 
	"${PROJECT_SOURCE_DIR}/include/imgui.h" 
	"${PROJECT_SOURCE_DIR}/include/imconfig.h" 
	"${PROJECT_SOURCE_DIR}/include/imgui_internal.h" 
	"${PROJECT_SOURCE_DIR}/include/imstb_rectpack.h" 
	"${PROJECT_SOURCE_DIR}/include/imstb_textedit.h" 
	"${PROJECT_SOURCE_DIR}/include/imstb_truetype.h"
)
list(APPEND SOURCES 
	"${PROJECT_SOURCE_DIR}/src/imgui_draw.cpp" 
	"${PROJECT_SOURCE_DIR}/src/imgui_tables.cpp" 
	"${PROJECT_SOURCE_DIR}/src/imgui_widgets.cpp" 
	"${PROJECT_SOURCE_DIR}/src/imgui.cpp"
)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include 
	CACHE INTERNAL "${PROJECT_NAME}: includes" FORCE)
	
set(${PROJECT_NAME}_LIBRARIES ${PROJECT_NAME}
	CACHE INTERNAL "${PROJECT_NAME}: libraries" FORCE)

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

source_group("include" FILES ${HEADERS})
source_group("src" FILES ${SOURCES})

fastcg_add_dependency_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})