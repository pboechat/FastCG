cmake_minimum_required(VERSION 3.20)

if(FASTCG_GRAPHICS_SYSTEM STREQUAL "Vulkan")
	find_package(Vulkan REQUIRED)
endif()

project(FastCG)

file(GLOB_RECURSE HEADERS 
	"${PROJECT_SOURCE_DIR}/include/**/*.h" 
	"${PROJECT_SOURCE_DIR}/include/*.h" 
	"${PROJECT_SOURCE_DIR}/include/**/*.hpp" 
	"${PROJECT_SOURCE_DIR}/include/*.hpp" 
	"${PROJECT_SOURCE_DIR}/include/**/*.inc" 
	"${PROJECT_SOURCE_DIR}/include/*.inc"
)
file(GLOB_RECURSE SOURCES 
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
	set(PLATFORM_INCLUDE_DIRS ${messagebox_x11_INCLUDE_DIRS} ${X11_INCLUDE} ${X11_Xrender_INCLUDE_PATH})
	set(PLATFORM_LIBRARIES ${messagebox_x11_LIBRARIES} ${X11_LIBRARIES} ${X11_Xrender_LIB})
elseif(FASTCG_PLATFORM STREQUAL "Android")
	set(PLATFORM_INCLUDE_DIRS ${android_native_app_glue_INCLUDE_DIRS})
	set(PLATFORM_LIBRARIES android log android_native_app_glue)
endif()

if(FASTCG_GRAPHICS_SYSTEM STREQUAL "OpenGL")
	if(FASTCG_PLATFORM STREQUAL "Android")
		set(GRAPHICS_SYSTEM_LIBRARIES EGL GLESv3)
	else()
		set(GRAPHICS_SYSTEM_INCLUDE_DIRS ${glew_INCLUDE_DIRS})
		set(GRAPHICS_SYSTEM_LIBRARIES ${glew_LIBRARIES})
		set(GRAPHICS_SYSTEM_DEFINES -DGLEW_STATIC)
	endif()
elseif(FASTCG_GRAPHICS_SYSTEM STREQUAL "Vulkan")
	set(GRAPHICS_SYSTEM_INCLUDE_DIRS ${Vulkan_INCLUDE_DIRS} ${VulkanMemoryAllocator_INCLUDE_DIRS})
	set(GRAPHICS_SYSTEM_LIBRARIES ${Vulkan_LIBRARIES} ${VulkanMemoryAllocator_LIBRARIES} spirv-cross-core)
endif()

set(${PROJECT_NAME}_INCLUDE_DIRS ${PLATFORM_INCLUDE_DIRS} ${GRAPHICS_SYSTEM_INCLUDE_DIRS} ${imgui_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include 
	CACHE INTERNAL "${PROJECT_NAME}: includes" FORCE)
	
set(LIBRARIES_INTERNAL ${PLATFORM_LIBRARIES} ${GRAPHICS_SYSTEM_LIBRARIES} glm imgui rapidjson stb tinyobj_loader_c)
	
set(${PROJECT_NAME}_LIBRARIES ${PROJECT_NAME} ${LIBRARIES_INTERNAL}
	CACHE INTERNAL "${PROJECT_NAME}: libraries" FORCE)

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

source_group("include" FILES ${HEADERS})
source_group("src" FILES ${SOURCES})
source_group("shaders/deferred" FILES ${DEFERRED_SHADERS})
source_group("shaders/forward" FILES ${FORWARD_SHADERS})
source_group("shaders" FILES ${GENERAL_SHADERS})

fastcg_add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES} ${SHADERS})
add_definitions(${PLATFORM_DEFINES} ${GRAPHICS_SYSTEM_DEFINES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES_INTERNAL})