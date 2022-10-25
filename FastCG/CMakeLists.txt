cmake_minimum_required(VERSION 3.8)

project(FastCG)

file(GLOB HEADERS 
	"${PROJECT_SOURCE_DIR}/include/**/*.h" 
	"${PROJECT_SOURCE_DIR}/include/*.h" 
	"${PROJECT_SOURCE_DIR}/include/**/*.hpp" 
	"${PROJECT_SOURCE_DIR}/include/*.hpp" 
	"${PROJECT_SOURCE_DIR}/include/**/*.inc" 
	"${PROJECT_SOURCE_DIR}/include/*.inc"
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

list(APPEND SHADERS FORWARD_SHADERS DEFERRED_SHADERS)

set(SHADERS ${FORWARD_SHADERS} ${DEFERRED_SHADERS} ${GENERAL_SHADERS})

if(FASTCG_PLATFORM STREQUAL "Linux")
	set(${PROJECT_NAME}_PLATFORM_SPECIFIC_INCLUDE_DIRS ${messagebox_x11_INCLUDE_DIRS} ${X11_INCLUDE} ${X11_Xrender_INCLUDE_PATH})
endif()

set(${PROJECT_NAME}_INCLUDE_DIRS ${glew_INCLUDE_DIRS} ${imgui_INCLUDE_DIRS} ${${PROJECT_NAME}_PLATFORM_SPECIFIC_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include 
	CACHE INTERNAL "${PROJECT_NAME}: includes" FORCE)

if(FASTCG_PLATFORM STREQUAL "Linux")
	set(${PROJECT_NAME}_PLATFORM_SPECIFIC_LIBRARIES ${messagebox_x11_LIBRARIES} ${X11_LIBRARIES} ${X11_Xrender_LIB})
endif()
	
set(${PROJECT_NAME}_LIBRARIES_INTERNAL ${glew_LIBRARIES} ${${PROJECT_NAME}_PLATFORM_SPECIFIC_LIBRARIES} glm imgui stb tinyobj_loader_c)
	
set(${PROJECT_NAME}_LIBRARIES ${PROJECT_NAME} ${${PROJECT_NAME}_LIBRARIES_INTERNAL}
	CACHE INTERNAL "${PROJECT_NAME}: libraries" FORCE)

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

source_group("include" FILES ${HEADERS})
source_group("src" FILES ${SOURCES})
source_group("shaders/deferred" FILES ${DEFERRED_SHADERS})
source_group("shaders/forward" FILES ${FORWARD_SHADERS})
source_group("shaders" FILES ${GENERAL_SHADERS})

fastcg_add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES} ${SHADERS})
add_definitions(-DGLEW_STATIC)
set_target_properties(${PROJECT_NAME} PROPERTIES 
	POSITION_INDEPENDENT_CODE CXX 
	LINKER_LANGUAGE CXX
)
target_link_libraries(${PROJECT_NAME} ${FastCG_LIBRARIES_INTERNAL})