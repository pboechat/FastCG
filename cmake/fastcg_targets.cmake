function(_fastcg_remove)
    foreach(FILE IN LISTS ARGN)
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E remove ${FILE}
        )
    endforeach()
endfunction()

function(_fastcg_compile_glsl_shaders)
    get_target_property(SOURCE_DIR ${ARGV0} SOURCE_DIR)

    set(SRC_SHADERS_DIR "${SOURCE_DIR}/assets/shaders")
    set(DST_SHADERS_DIR "${FASTCG_DEPLOY}/assets/${ARGV0}/shaders")

    set(FASTCG_SHADER_COMPILER_ARGS -e main --source-entrypoint main)

    if(FASTCG_GRAPHICS_SYSTEM STREQUAL "OpenGL")
        set(FASTCG_SHADER_COMPILER_ARGS ${FASTCG_SHADER_COMPILER_ARGS} --target-env opengl)
    elseif(FASTCG_GRAPHICS_SYSTEM STREQUAL "Vulkan")
        set(FASTCG_SHADER_COMPILER_ARGS ${FASTCG_SHADER_COMPILER_ARGS} --target-env vulkan1.3)
    else()
        message(FATAL_ERROR "Don't know how to compile GLSL shaders for ${FASTCG_GRAPHICS_SYSTEM}")
    endif()

    set(FASTCG_SHADER_COMPILER_ARGS ${FASTCG_SHADER_COMPILER_ARGS} -DENABLE_INCLUDE_EXTENSION_DIRECTIVE)

    file(GLOB_RECURSE GLSL_HEADERS "${SRC_SHADERS_DIR}/*.glsl")
    file(GLOB_RECURSE GLSL_SOURCES "${SRC_SHADERS_DIR}/*.vert" "${SRC_SHADERS_DIR}/*.frag")
    foreach(GLSL_SOURCE IN LISTS GLSL_SOURCES)
        file(RELATIVE_PATH REL_GLSL_SOURCE ${SRC_SHADERS_DIR} ${GLSL_SOURCE})
        get_filename_component(GLSL_SOURCE_DIR ${REL_GLSL_SOURCE} DIRECTORY)
        get_filename_component(GLSL_SOURCE_BASENAME ${REL_GLSL_SOURCE} NAME_WE)
        get_filename_component(GLSL_SOURCE_EXT ${REL_GLSL_SOURCE} EXT)
        string(SUBSTRING ${GLSL_SOURCE_EXT} 1 -1 GLSL_SOURCE_EXT)
        set(SPIRV_DIR ${DST_SHADERS_DIR}/${GLSL_SOURCE_DIR})
        set(SPIRV_FILE "${SPIRV_DIR}/${GLSL_SOURCE_BASENAME}.${GLSL_SOURCE_EXT}_spv")
        add_custom_command(
            OUTPUT ${SPIRV_FILE}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${SPIRV_DIR}
            COMMAND ${FASTCG_GLSLANGVALIDATOR} ${FASTCG_SHADER_COMPILER_ARGS} ${GLSL_SOURCE} -o ${SPIRV_FILE} $<IF:$<CONFIG:Debug>,-g,>  # generate debug info if in Debug config
            DEPENDS ${GLSL_SOURCE} ${GLSL_HEADERS}
        )
        list(APPEND GLSL_SPIRV_FILES ${SPIRV_FILE})
    endforeach(GLSL_SOURCE)
    if(GLSL_SPIRV_FILES)
        add_custom_target(
            ${ARGV0}_FASTCG_COMPILE_GLSL_SHADERS
            DEPENDS ${GLSL_SPIRV_FILES}
        )
        add_dependencies(${ARGV0} ${ARGV0}_FASTCG_COMPILE_GLSL_SHADERS)
    endif()
endfunction()

function(_fastcg_copy_assets)
    get_target_property(SOURCE_DIR ${ARGV0} SOURCE_DIR)

    set(SRC_ASSETS_DIR "${SOURCE_DIR}/assets")
    set(DST_ASSETS_DIR "${FASTCG_DEPLOY}/assets/${ARGV0}")

    file(GLOB_RECURSE SRC_ASSET_FILES "${SRC_ASSETS_DIR}/*.*")
    foreach(SRC_ASSET_FILE IN LISTS SRC_ASSET_FILES)
        file(RELATIVE_PATH REL_ASSET_FILE ${SRC_ASSETS_DIR} ${SRC_ASSET_FILE})
        if(REL_ASSET_FILE MATCHES "shaders" AND NOT FASTCG_USE_TEXT_SHADERS)
            continue()
        endif()
        set(DST_ASSET_FILE "${DST_ASSETS_DIR}/${REL_ASSET_FILE}")
        add_custom_command(
            OUTPUT ${DST_ASSET_FILE}
            COMMAND ${CMAKE_COMMAND} -E copy ${SRC_ASSET_FILE} ${DST_ASSET_FILE}
            DEPENDS ${SRC_ASSET_FILE}
        )
        list(APPEND DST_ASSET_FILES ${DST_ASSET_FILE})
    endforeach(SRC_ASSET_FILE)
    if(DST_ASSET_FILES)
        add_custom_target(
            ${ARGV0}_FASTCG_COPY_ASSETS
            DEPENDS ${DST_ASSET_FILES}
        )
        add_dependencies(${ARGV0} ${ARGV0}_FASTCG_COPY_ASSETS)
    endif()
endfunction()

function(_fastcg_prepare_assets)
    if(NOT FASTCG_USE_TEXT_SHADERS)
        _fastcg_compile_glsl_shaders(${ARGV})
    endif()
    _fastcg_copy_assets(${ARGV})
endfunction()

function(_fastcg_add_definitions)
    add_definitions(-DFASTCG_PLATFORM="${FASTCG_PLATFORM}")
    if(FASTCG_PLATFORM STREQUAL "Windows")
        add_definitions(-DFASTCG_WINDOWS)
    elseif(FASTCG_PLATFORM STREQUAL "Linux")
        add_definitions(-DFASTCG_LINUX)
    endif()
    add_definitions(-DFASTCG_GRAPHICS_SYSTEM="${FASTCG_GRAPHICS_SYSTEM}")
    if(FASTCG_GRAPHICS_SYSTEM STREQUAL "OpenGL")
        add_definitions(-DFASTCG_OPENGL)
    elseif(FASTCG_GRAPHICS_SYSTEM STREQUAL "Vulkan")
        # FIXME: use [0, 1] depth!
        add_definitions(-DFASTCG_VULKAN) # -DGLM_FORCE_DEPTH_ZERO_TO_ONE)
    endif()
    if (FASTCG_DISABLE_GPU_TIMING)
        add_definitions(-DFASTCG_DISABLE_GPU_TIMING)
    endif()
    if (FASTCG_DISABLE_GPU_VALIDATION)
        add_definitions(-DFASTCG_DISABLE_GPU_VALIDATION)
    endif()
    if(FASTCG_PLATFORM STREQUAL "Linux")
        target_compile_definitions(${ARGV0} PUBLIC $<IF:$<CONFIG:Debug>,_DEBUG=1,>)
    endif()
endfunction()

function(_fastcg_set_target_properties)
    set(DEPLOY_DIR "${FASTCG_DEPLOY}/${ARGV0}")
    set_target_properties(${ARGV0} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${DEPLOY_DIR}
        DEBUG_POSTFIX "d"
    )

    foreach(CMAKE_CONFIGURATION_TYPE ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${CMAKE_CONFIGURATION_TYPE} CMAKE_CONFIGURATION_TYPE)
        set_target_properties(${ARGV0} PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY_${CMAKE_CONFIGURATION_TYPE} ${DEPLOY_DIR}
            VS_DEBUGGER_WORKING_DIRECTORY ${DEPLOY_DIR}
            DEBUG_POSTFIX "d"
        )
    endforeach(CMAKE_CONFIGURATION_TYPE)
endfunction()

function(_fastcg_add_library)
    add_library(${ARGN})
    _fastcg_set_target_properties(${ARGV0})
    _fastcg_prepare_assets(${ARGV0})
endfunction()

function(fastcg_add_executable)
    add_executable(${ARGN})
    _fastcg_add_definitions(${ARGV0})
    _fastcg_set_target_properties(${ARGV0})
    target_link_libraries(${ARGV0} ${FastCG_LIBRARIES})
    _fastcg_prepare_assets(${ARGV0})
endfunction()

function(fastcg_add_dependency_library)
    _fastcg_add_library(${ARGN})
endfunction()

function(fastcg_add_library)
    _fastcg_add_library(${ARGN})
    _fastcg_add_definitions(${ARGV0})
endfunction()

if(DEFINED FASTCG_EXEC)
    separate_arguments(FASTCG_EXEC_ARGS)
    if(FASTCG_EXEC STREQUAL "remove")
        _fastcg_remove(${FASTCG_EXEC_ARGS})
    endif()
endif()