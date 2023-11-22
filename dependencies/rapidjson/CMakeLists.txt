cmake_minimum_required(VERSION 3.20)

project(rapidjson)

file(GLOB HEADERS 
	"${PROJECT_SOURCE_DIR}/include/**/*.h" 
	"${PROJECT_SOURCE_DIR}/include/*.h" 
)

source_group("include" FILES ${HEADERS})

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${PROJECT_SOURCE_DIR}/include)