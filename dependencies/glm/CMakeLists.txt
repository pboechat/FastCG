cmake_minimum_required(VERSION 3.0)

project(glm)

file(GLOB HEADERS 
	"${PROJECT_SOURCE_DIR}/include/**/*.h" 
	"${PROJECT_SOURCE_DIR}/include/*.h" 
	"${PROJECT_SOURCE_DIR}/include/**/*.hpp" 
	"${PROJECT_SOURCE_DIR}/include/*.hpp" 
	"${PROJECT_SOURCE_DIR}/src/**/*.h" 
	"${PROJECT_SOURCE_DIR}/src/*.h" 
	"${PROJECT_SOURCE_DIR}/src/**/*.hpp"
	"${PROJECT_SOURCE_DIR}/src/*.hpp"
)

source_group("include" FILES ${HEADERS})

add_library(${PROJECT_NAME} INTERFACE ${HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
target_include_directories(${PROJECT_NAME} INTERFACE include/)