cmake_minimum_required(VERSION 3.20)

project(astcenc)

list(APPEND HEADERS "${PROJECT_SOURCE_DIR}/include/astcenc.h")
list(APPEND SOURCES 
	"${PROJECT_SOURCE_DIR}/src/astcenccli_entry.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenccli_error_metrics.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenccli_image.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenccli_image_external.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenccli_image_load_store.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenccli_platform_dependents.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenccli_toplevel.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenccli_toplevel_help.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenc_averages_and_directions.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenc_block_sizes.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenc_color_quantize.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenc_color_unquantize.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenc_compress_symbolic.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenc_compute_variance.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenc_decompress_symbolic.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenc_diagnostic_trace.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenc_entry.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenc_find_best_partitioning.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenc_ideal_endpoints_and_weights.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenc_image.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenc_integer_sequence.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenc_mathlib.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenc_mathlib_softfloat.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenc_partition_tables.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenc_percentile_tables.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenc_pick_best_endpoint_format.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenc_platform_isa_detection.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenc_quantization.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenc_symbolic_physical.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenc_weight_align.cpp"
	"${PROJECT_SOURCE_DIR}/src/astcenc_weight_quant_xfer_tables.cpp"
	"${PROJECT_SOURCE_DIR}/src/wuffs-v0.3.c"
)

source_group("include" FILES ${HEADERS})
source_group("src" FILES ${SOURCES})

fastcg_add_dependency_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} stb)