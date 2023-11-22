cmake_minimum_required(VERSION 3.20)

project(basisu)

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/include/**/*.h" "${PROJECT_SOURCE_DIR}/src/**/*.h")
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

source_group("include" FILES ${HEADERS})
source_group("src" FILES ${SOURCES})

fastcg_add_dependency_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(
	${PROJECT_NAME}
	PRIVATE
		BASISD_SUPPORT_FXT1=0 # to reduce size, don't support transcoding to ancient formats
		BASISD_SUPPORT_KTX2_ZSTD=0
		BASISD_SUPPORT_KTX2=1
		BASISU_SUPPORT_SSE=0 # TODO:
		BASISU_SUPPORT_OPENCL=0
)
target_link_libraries(${PROJECT_NAME} zlib)