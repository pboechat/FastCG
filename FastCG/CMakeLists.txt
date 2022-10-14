cmake_minimum_required(VERSION 3.8)

project(FastCG)

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
file(GLOB SOURCES 
	"${PROJECT_SOURCE_DIR}/src/**/*.cpp" 
	"${PROJECT_SOURCE_DIR}/src/*.cpp" 
	"${PROJECT_SOURCE_DIR}/src/**/*.c"
	"${PROJECT_SOURCE_DIR}/src/*.c"
)

file(GLOB FORWARD_SHADERS 
	"${PROJECT_SOURCE_DIR}/../assets/shaders/forward/*.vert" 
	"${PROJECT_SOURCE_DIR}/../assets/shaders/forward/*.frag" 
	"${PROJECT_SOURCE_DIR}/../assets/shaders/forward/*.glsl" 
)

file(GLOB DEFERRED_SHADERS 
	"${PROJECT_SOURCE_DIR}/../assets/shaders/deferred/*.vert" 
	"${PROJECT_SOURCE_DIR}/../assets/shaders/deferred/*.frag" 
	"${PROJECT_SOURCE_DIR}/../assets/shaders/deferred/*.glsl" 
)

file(GLOB GENERAL_SHADERS 
	"${PROJECT_SOURCE_DIR}/../assets/shaders/*.vert" 
	"${PROJECT_SOURCE_DIR}/../assets/shaders/*.frag" 
	"${PROJECT_SOURCE_DIR}/../assets/shaders/*.glsl" 
)

set(SHADERS ${FORWARD_SHADERS} ${DEFERRED_SHADERS} ${GENERAL_SHADERS})

set(${PROJECT_NAME}_INCLUDE_DIRS ${glew_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include 
	CACHE INTERNAL "${PROJECT_NAME}: includes" FORCE)

set(${PROJECT_NAME}_LIBRARIES_INTERNAL ${glew_LIBRARIES} glm stb tinyobj_loader_c)
	
set(${PROJECT_NAME}_LIBRARIES ${PROJECT_NAME} ${${PROJECT_NAME}_LIBRARIES_INTERNAL}
	CACHE INTERNAL "${PROJECT_NAME}: libraries" FORCE)
message("RENDERING_SYSTEM=${RENDERING_SYSTEM}")

if(RENDERING_SYSTEM STREQUAL "OpenGL")
    add_definitions(-DFASTCG_OPENGL)
endif()
add_definitions(-DGLEW_STATIC)

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

source_group("include" FILES ${HEADERS})
source_group("src" FILES ${SOURCES})
source_group("shaders/deferred" FILES ${DEFERRED_SHADERS})
source_group("shaders/forward" FILES ${FORWARD_SHADERS})
source_group("shaders" FILES ${GENERAL_SHADERS})

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES} ${SHADERS})

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d" POSITION_INDEPENDENT_CODE CXX LINKER_LANGUAGE CXX)

target_link_libraries(${PROJECT_NAME} ${FastCG_LIBRARIES_INTERNAL})
