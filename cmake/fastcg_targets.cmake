function(_fastcg_compile_shaders)
    get_target_property(SOURCE_DIR ${ARGV0} SOURCE_DIR)
    get_target_property(BINARY_DIR ${ARGV0} BINARY_DIR)

    set(SPIRV_DIR "${BINARY_DIR}/spirv")
    set(SRC_SHADERS_DIR "${SOURCE_DIR}/assets/shaders")
    set(DST_SHADERS_DIR "${FASTCG_DEPLOY}/assets/${ARGV0}/shaders")

    file(GLOB_RECURSE GLSL_FILES "${SRC_SHADERS_DIR}/*.vert" "${SRC_SHADERS_DIR}/*.frag")
    foreach(GLSL_FILE IN LISTS GLSL_FILES)
        file(RELATIVE_PATH REL_GLSL_FILE ${SRC_SHADERS_DIR} ${GLSL_FILE})
        set(SPIRV_FILE "${SPIRV_DIR}/${REL_GLSL_FILE}.spv")
        add_custom_command(
            OUTPUT ${SPIRV_FILE}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${SPIRV_DIR}"
            COMMAND glslangValidator -V ${GLSL_FILE} -o ${SPIRV_FILE}
            DEPENDS ${GLSL_FILE}
        )
        list(APPEND GLSL_SPIRV_FILES ${SPIRV_FILE})
    endforeach(GLSL_FILE)
    if(GLSL_SPIRV_FILES)
        add_custom_target(
            ${ARGV0}_FASTCG_COMPILE_SHADERS
            COMMAND ${CMAKE_COMMAND} -E make_directory "${DST_SHADERS_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${SPIRV_DIR}" "${DST_SHADERS_DIR}"
            DEPENDS ${GLSL_SPIRV_FILES}
        )
        add_dependencies(${ARGV0} ${ARGV0}_FASTCG_COMPILE_SHADERS)
    endif()
endfunction()

function(_fastcg_copy_assets)
    get_target_property(SOURCE_DIR ${ARGV0} SOURCE_DIR)

    set(SRC_ASSETS_DIR "${SOURCE_DIR}/assets")
    set(DST_ASSETS_DIR "${FASTCG_DEPLOY}/assets/${ARGV0}")

    file(GLOB_RECURSE SRC_ASSET_FILES "${SRC_ASSETS_DIR}/*.*")
    foreach(SRC_ASSET_FILE IN LISTS SRC_ASSET_FILES)
        file(RELATIVE_PATH REL_ASSET_FILE ${SRC_ASSETS_DIR} ${SRC_ASSET_FILE})
        if(NOT FASTCG_RENDERING_SYSTEM STREQUAL "OpenGL" AND REL_ASSET_FILE MATCHES "shaders")
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
    if(NOT FASTCG_RENDERING_SYSTEM STREQUAL "OpenGL")
        _fastcg_compile_shaders(${ARGV0})
    endif()
    _fastcg_copy_assets(${ARGV})
endfunction()

function(_fastcg_add_definitions)
    if(FASTCG_PLATFORM STREQUAL "Windows")
        add_definitions(-DFASTCG_WINDOWS)
    elseif(FASTCG_PLATFORM STREQUAL "Linux")
        add_definitions(-DFASTCG_LINUX)
    endif()
    if(FASTCG_RENDERING_SYSTEM STREQUAL "OpenGL")
        add_definitions(-DFASTCG_OPENGL)
    endif()
endfunction()

function(_fastcg_set_target_properties)
    set(DEPLOY_DIR "${FASTCG_DEPLOY}/${ARGV0}")
    set(LIBS_DIR "${FASTCG_LIBS}/${ARGV0}")

    foreach(CMAKE_CONFIGURATION_TYPE ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${CMAKE_CONFIGURATION_TYPE} CMAKE_CONFIGURATION_TYPE)
        set_target_properties(${ARGV0} PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY_${CMAKE_CONFIGURATION_TYPE} ${DEPLOY_DIR}
            ARCHIVE_OUTPUT_DIRECTORY_${CMAKE_CONFIGURATION_TYPE} ${LIBS_DIR}
            LIBRARY_OUTPUT_DIRECTORY_${CMAKE_CONFIGURATION_TYPE} ${LIBS_DIR}
            VS_DEBUGGER_WORKING_DIRECTORY ${DEPLOY_DIR}
            DEBUG_POSTFIX "d"
        )
    endforeach(CMAKE_CONFIGURATION_TYPE CMAKE_CONFIGURATION_TYPES)
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