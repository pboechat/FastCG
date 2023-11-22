cmake_minimum_required(VERSION 3.20)

project(dfdutils)

find_package(Vulkan REQUIRED)

list(APPEND HEADERS 
	"${PROJECT_SOURCE_DIR}/include/dfdutils/dfd.h"
	"${PROJECT_SOURCE_DIR}/include/dfdutils/KHR/khr_df.h"
)
list(APPEND SOURCES 
	"${PROJECT_SOURCE_DIR}/src/colourspaces.c"
	"${PROJECT_SOURCE_DIR}/src/createdfd.c"
	"${PROJECT_SOURCE_DIR}/src/dfd2vk.c"
	"${PROJECT_SOURCE_DIR}/src/interpretdfd.c"
	"${PROJECT_SOURCE_DIR}/src/printdfd.c"
	"${PROJECT_SOURCE_DIR}/src/queries.c"
	"${PROJECT_SOURCE_DIR}/src/vk2dfd.c"
)
	
source_group("include" FILES ${HEADERS})
source_group("src" FILES ${SOURCES})
	
fastcg_add_dependency_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES})