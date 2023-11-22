cmake_minimum_required(VERSION 3.20)

project(stb)

list(APPEND HEADERS 
	"${PROJECT_SOURCE_DIR}/include/stb_image.h"
	"${PROJECT_SOURCE_DIR}/include/stb_image_write.h"
	"${PROJECT_SOURCE_DIR}/include/stb_truetype.h"
)
list(APPEND SOURCES "${PROJECT_SOURCE_DIR}/src/stb.c")

source_group("include" FILES ${HEADERS})
source_group("src" FILES ${SOURCES})

fastcg_add_dependency_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(
	${PROJECT_NAME} 
	PRIVATE 
		STB_IMAGE_IMPLEMENTATION 
		STB_IMAGE_WRITE_IMPLEMENTATION 
)