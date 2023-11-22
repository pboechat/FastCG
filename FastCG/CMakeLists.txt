cmake_minimum_required(VERSION 3.20)

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
	set(PLATFORM_LIBRARIES messagebox-x11)
elseif(FASTCG_PLATFORM STREQUAL "Android")
	set(PLATFORM_LIBRARIES android log android_native_app_glue)
endif()

if(FASTCG_GRAPHICS_SYSTEM STREQUAL "OpenGL")
	if(FASTCG_PLATFORM STREQUAL "Android")
		set(GRAPHICS_SYSTEM_LIBRARIES EGL GLESv3)
	else()
		set(GRAPHICS_SYSTEM_LIBRARIES glew)
	endif()
elseif(FASTCG_GRAPHICS_SYSTEM STREQUAL "Vulkan")
	set(GRAPHICS_SYSTEM_LIBRARIES VulkanMemoryAllocator spirv-cross-core)
endif()

source_group("include" FILES ${HEADERS})
source_group("src" FILES ${SOURCES})
source_group("shaders/deferred" FILES ${DEFERRED_SHADERS})
source_group("shaders/forward" FILES ${FORWARD_SHADERS})
source_group("shaders" FILES ${GENERAL_SHADERS})

fastcg_add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES} ${SHADERS})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} ${PLATFORM_LIBRARIES} ${GRAPHICS_SYSTEM_LIBRARIES} glm imgui ktx rapidjson stb tinyobj_loader_c)