cmake_minimum_required(VERSION 3.20)

project(ktx)

find_package(Vulkan REQUIRED)

list(APPEND HEADERS 
	"${PROJECT_SOURCE_DIR}/include/ktx.h"
	"${PROJECT_SOURCE_DIR}/include/ktxvulkan.h"
)
list(APPEND SOURCES 
	"${PROJECT_SOURCE_DIR}/src/astc_encode.cpp"
	"${PROJECT_SOURCE_DIR}/src/basis_encode.cpp"
	"${PROJECT_SOURCE_DIR}/src/basis_transcode.cpp"
	"${PROJECT_SOURCE_DIR}/src/miniz_wrapper.cpp"
	"${PROJECT_SOURCE_DIR}/src/checkheader.c"
	"${PROJECT_SOURCE_DIR}/src/filestream.c"
	"${PROJECT_SOURCE_DIR}/src/glloader.c"
	"${PROJECT_SOURCE_DIR}/src/gl_funcs.c"
	"${PROJECT_SOURCE_DIR}/src/hashlist.c"
	"${PROJECT_SOURCE_DIR}/src/info.c"
	"${PROJECT_SOURCE_DIR}/src/memstream.c"
	"${PROJECT_SOURCE_DIR}/src/strings.c"
	"${PROJECT_SOURCE_DIR}/src/swap.c"
	"${PROJECT_SOURCE_DIR}/src/texture.c"
	"${PROJECT_SOURCE_DIR}/src/texture1.c"
	"${PROJECT_SOURCE_DIR}/src/texture2.c"
	"${PROJECT_SOURCE_DIR}/src/vkformat_check.c"
	"${PROJECT_SOURCE_DIR}/src/vkformat_str.c"
	"${PROJECT_SOURCE_DIR}/src/vkloader.c"
	"${PROJECT_SOURCE_DIR}/src/vk_funcs.c"
	"${PROJECT_SOURCE_DIR}/src/writer1.c"
	"${PROJECT_SOURCE_DIR}/src/writer2.c"
)

source_group("include" FILES ${HEADERS})
source_group("src" FILES ${SOURCES})

fastcg_add_dependency_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(
	${PROJECT_NAME}
	PUBLIC
		KHRONOS_STATIC 
	PRIVATE
		KTX_FEATURE_KTX1 
		KTX_FEATURE_KTX2 
		KTX_FEATURE_WRITE # enable write support
)
target_link_libraries(${PROJECT_NAME} astcenc basisu dfdutils zlib)