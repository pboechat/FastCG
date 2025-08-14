# all glsl source extensions
set(GLSL_SOURCE_EXTS ".vert" ".frag" ".comp")

function(_fastcg_add_compile_shaders_target)
    get_target_property(SOURCE_DIR ${ARGV0} SOURCE_DIR)

    set(SRC_SHADERS_DIR "${SOURCE_DIR}/assets/shaders")
    set(DST_SHADERS_DIR "${FASTCG_DEPLOY}/assets/${ARGV0}/shaders")
    set(TMP_SHADERS_DIR "${CMAKE_BINARY_DIR}/assets/shaders/${ARGV0}")

    set(SHADER_COMPILER_ARGS -e main --source-entrypoint main)

    if(FASTCG_PLATFORM STREQUAL "Android")
        set(GLSL_VERSION "320 es")
    else()
        set(GLSL_VERSION "430")
    endif()

    if(FASTCG_GRAPHICS_SYSTEM STREQUAL "OpenGL")
        set(SHADER_COMPILER_ARGS ${SHADER_COMPILER_ARGS} --target-env opengl)
    else()
        set(SHADER_COMPILER_ARGS ${SHADER_COMPILER_ARGS} --target-env vulkan1.1)
    endif()

    file(GLOB_RECURSE GLSL_HEADERS "${SRC_SHADERS_DIR}/*.glsl")
    foreach(GLSL_HEADER IN LISTS GLSL_HEADERS)
        file(RELATIVE_PATH REL_GLSL_HEADER ${SRC_SHADERS_DIR} ${GLSL_HEADER})
        set(DST_GLSL_HEADER "${DST_SHADERS_DIR}/${REL_GLSL_HEADER}")
        set(TMP_GLSL_HEADER "${TMP_SHADERS_DIR}/${REL_GLSL_HEADER}")
        if(FASTCG_USE_TEXT_SHADERS)
            set(GLSL_HEADER_ ${DST_GLSL_HEADER})
        else()
            set(GLSL_HEADER_ ${TMP_GLSL_HEADER})
        endif()
        add_custom_command(
            OUTPUT ${GLSL_HEADER_}
            COMMAND ${CMAKE_COMMAND} -E copy ${GLSL_HEADER} ${GLSL_HEADER_} # copy headers to either destination or tmp directories
            DEPENDS ${GLSL_HEADER}
        )
        list(APPEND DST_GLSL_HEADERS ${DST_GLSL_HEADER})
        list(APPEND TMP_GLSL_HEADERS ${TMP_GLSL_HEADER})
    endforeach(GLSL_HEADER)

    foreach(GLSL_SOURCE_EXT IN LISTS GLSL_SOURCE_EXTS)
        list(APPEND GLSL_SOURCE_PATTERNS "${SRC_SHADERS_DIR}/*${GLSL_SOURCE_EXT}")
    endforeach(GLSL_SOURCE_EXT)

    file(GLOB_RECURSE GLSL_SOURCES ${GLSL_SOURCE_PATTERNS})
    foreach(GLSL_SOURCE IN LISTS GLSL_SOURCES)
        file(RELATIVE_PATH REL_GLSL_SOURCE ${SRC_SHADERS_DIR} ${GLSL_SOURCE})

        get_filename_component(GLSL_SOURCE_DIR ${REL_GLSL_SOURCE} DIRECTORY)
        get_filename_component(GLSL_SOURCE_BASENAME ${REL_GLSL_SOURCE} NAME_WE)
        get_filename_component(GLSL_SOURCE_EXT ${REL_GLSL_SOURCE} EXT)
        
        string(SUBSTRING ${GLSL_SOURCE_EXT} 1 -1 GLSL_SOURCE_EXT)
        set(DST_TEXT_SOURCE "${DST_SHADERS_DIR}/${REL_GLSL_SOURCE}")
        set(DST_BINARY_SOURCE "${DST_SHADERS_DIR}/${GLSL_SOURCE_DIR}/${GLSL_SOURCE_BASENAME}.${GLSL_SOURCE_EXT}_spv")
        
        if(FASTCG_USE_TEXT_SHADERS)
            set(DST_GLSL_SOURCE ${DST_TEXT_SOURCE})
            add_custom_command(
                OUTPUT ${DST_GLSL_SOURCE}
                COMMAND ${CMAKE_COMMAND} -E rm -f ${DST_BINARY_SOURCE} # clean up binary source in the destination directory (just in case)
                COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/cmake/fastcg_glsl_processor.cmake ${FASTCG_PLATFORM} "${GLSL_SOURCE}" "${DST_GLSL_SOURCE}" "${GLSL_VERSION}" # process text source and store it in the destination directory
                DEPENDS ${GLSL_SOURCE} ${DST_GLSL_HEADERS}
            )
        else()
            set(TMP_GLSL_SOURCE "${TMP_SHADERS_DIR}/${REL_GLSL_SOURCE}")
            set(DST_GLSL_SOURCE ${DST_BINARY_SOURCE})
            get_filename_component(DST_GLSL_SOURCE_DIR ${DST_GLSL_SOURCE} DIRECTORY)
            add_custom_command(
                OUTPUT ${DST_GLSL_SOURCE}
                COMMAND ${CMAKE_COMMAND} -E rm -f ${DST_TEXT_SOURCE} # clean up text source in the destination directory (just in case)
                COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/cmake/fastcg_glsl_processor.cmake ${FASTCG_PLATFORM} "${GLSL_SOURCE}" "${TMP_GLSL_SOURCE}" "${GLSL_VERSION}" # process text source and store it in a tmp directory
                COMMAND ${CMAKE_COMMAND} -E make_directory ${DST_GLSL_SOURCE_DIR}
                # FIXME: can't compile non-debug shaders with -g0 otherwise runtime reflection doesn't work
                COMMAND ${FASTCG_GLSLANGVALIDATOR} ${SHADER_COMPILER_ARGS} ${TMP_GLSL_SOURCE} -o ${DST_GLSL_SOURCE} $<IF:$<CONFIG:Debug>,-g,>  # generate binary source in the destination directory
                DEPENDS ${GLSL_SOURCE} ${TMP_GLSL_HEADERS}
            )
        endif()
        list(APPEND DST_GLSL_SOURCES ${DST_GLSL_SOURCE})
    endforeach(GLSL_SOURCE)

    if(DST_GLSL_SOURCES)
        add_custom_target(
            ${ARGV0}_COMPILE_SHADERS
            DEPENDS ${DST_GLSL_SOURCES}
        )
        if(NOT FASTCG_USE_TEXT_SHADERS AND DST_GLSL_HEADERS)
            # clean up text headers in the destination directory (just in case)
            add_custom_command(
                TARGET ${ARGV0}_COMPILE_SHADERS PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E rm -f ${DST_GLSL_HEADERS}
            )
        endif()
        add_dependencies(${ARGV0} ${ARGV0}_COMPILE_SHADERS)
    endif()
endfunction()

function(_fastcg_add_cook_assets_target)
    get_target_property(SOURCE_DIR ${ARGV0} SOURCE_DIR)

    set(SRC_ASSETS_DIR "${SOURCE_DIR}/assets")
    set(DST_ASSETS_DIR "${FASTCG_DEPLOY}/assets/${ARGV0}")

    file(GLOB_RECURSE RECIPES "${SRC_ASSETS_DIR}/*.recipe")
    foreach(RECIPE IN LISTS RECIPES)
        file(RELATIVE_PATH REL_RECIPE_PATH ${SRC_ASSETS_DIR} ${RECIPE})
        string(REGEX REPLACE "(.*)\\.recipe$" "\\1" REL_COOKED_ASSET_PATH "${REL_RECIPE_PATH}")
        set(COOKED_ASSET "${DST_ASSETS_DIR}/${REL_COOKED_ASSET_PATH}")
        get_filename_component(COOKED_ASSET_DIR ${COOKED_ASSET} DIRECTORY)
        add_custom_command(
            OUTPUT ${COOKED_ASSET}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${COOKED_ASSET_DIR}
            COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/cmake/fastcg_asset_cooker.cmake "${SOURCE_DIR}" "${RECIPE}" "${COOKED_ASSET}"
            DEPENDS ${RECIPE}
        )
        list(APPEND COOKED_ASSETS ${COOKED_ASSET})
    endforeach()

    if(COOKED_ASSETS)
        add_custom_target(
            ${ARGV0}_COOK_ASSETS
            DEPENDS ${COOKED_ASSETS}
        )
        add_dependencies(${ARGV0} ${ARGV0}_COOK_ASSETS)
    endif()
endfunction()

function(_fastcg_add_copy_assets_target)
    get_target_property(SOURCE_DIR ${ARGV0} SOURCE_DIR)

    set(SRC_ASSETS_DIR "${SOURCE_DIR}/assets")
    set(DST_ASSETS_DIR "${FASTCG_DEPLOY}/assets/${ARGV0}")

    file(GLOB_RECURSE SRC_ASSET_FILES "${SRC_ASSETS_DIR}/*.*")
    foreach(SRC_ASSET_FILE IN LISTS SRC_ASSET_FILES)
        file(RELATIVE_PATH REL_ASSET_FILE ${SRC_ASSETS_DIR} ${SRC_ASSET_FILE})
        if(REL_ASSET_FILE MATCHES "\\.vert$" OR 
           REL_ASSET_FILE MATCHES "\\.frag$" OR 
           REL_ASSET_FILE MATCHES "\\.comp$" OR 
           REL_ASSET_FILE MATCHES "\\.glsl$")
            continue()
        endif()
        if(REL_ASSET_FILE MATCHES "\\.recipe$")
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
            ${ARGV0}_COPY_ASSETS
            DEPENDS ${DST_ASSET_FILES}
        )
        add_dependencies(${ARGV0} ${ARGV0}_COPY_ASSETS)
    endif()
endfunction()

function(_fastcg_add_asset_targets)
    _fastcg_add_compile_shaders_target(${ARGV})
    _fastcg_add_cook_assets_target(${ARGV})
    _fastcg_add_copy_assets_target(${ARGV})
endfunction()

function(_fastcg_target_compile_definitions)
    target_compile_definitions(${ARGV0} PUBLIC 
        FASTCG_PLATFORM="${FASTCG_PLATFORM}")
    if(FASTCG_PLATFORM STREQUAL "Windows")
        target_compile_definitions(${ARGV0} PUBLIC 
            FASTCG_WINDOWS 
            _CRT_SECURE_NO_WARNINGS)
    elseif(FASTCG_PLATFORM STREQUAL "Linux")
        target_compile_definitions(${ARGV0} PUBLIC 
            FASTCG_LINUX 
            FASTCG_POSIX)
    elseif(FASTCG_PLATFORM STREQUAL "Android")
        target_compile_definitions(${ARGV0} PUBLIC 
            FASTCG_ANDROID 
            FASTCG_POSIX)
    endif()
    target_compile_definitions(${ARGV0} PUBLIC 
        FASTCG_GRAPHICS_SYSTEM="${FASTCG_GRAPHICS_SYSTEM}")
    if(FASTCG_GRAPHICS_SYSTEM STREQUAL "OpenGL")
        target_compile_definitions(${ARGV0} PUBLIC 
            FASTCG_OPENGL)
    elseif(FASTCG_GRAPHICS_SYSTEM STREQUAL "Vulkan")
        # FIXME: use [0, 1] depth!
        target_compile_definitions(${ARGV0} PUBLIC 
            FASTCG_VULKAN) # GLM_FORCE_DEPTH_ZERO_TO_ONE)
    endif()
    if(FASTCG_DISABLE_GPU_TIMING)
        target_compile_definitions(${ARGV0} PUBLIC 
            FASTCG_DISABLE_GPU_TIMING)
    endif()
    if(FASTCG_DISABLE_GPU_VALIDATION)
        target_compile_definitions(${ARGV0} PUBLIC 
            FASTCG_DISABLE_GPU_VALIDATION)
    endif()
    if(FASTCG_ENABLE_VERBOSE_LOGGING)
        target_compile_definitions(${ARGV0} PUBLIC 
            FASTCG_LOG_SEVERITY=4)
    endif()
    target_compile_definitions(${ARGV0} PUBLIC 
        $<IF:$<CONFIG:Debug>,_DEBUG=1,>)
endfunction()

function(_fastcg_target_compile_options)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${ARGV0} PUBLIC 
            /W4 
            /permissive-
            /WX
            /wd4100 # unreferenced formal parameter
            /wd4201 # nameless struct/union
            /wd4505 # unreferenced local function has been removed
            /wd4702 # unreachable code
        )
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${ARGV0} PUBLIC 
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -Wno-gnu-zero-variadic-macro-arguments
            -Wno-implicit-fallthrough
            -Wno-nested-anon-types
            -Wno-missing-braces
            -Wno-missing-field-initializers
            -Wno-unused-function
            -Wno-unused-parameter
        )
    endif()
endfunction()

function(_fastcg_set_target_properties)
    set(DEPLOY_DIR "${FASTCG_DEPLOY}/${ARGV0}")

    set_target_properties(${ARGV0} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${DEPLOY_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${DEPLOY_DIR}
        DEBUG_POSTFIX "d"
    )

    foreach(CMAKE_CONFIGURATION_TYPE ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${CMAKE_CONFIGURATION_TYPE} CMAKE_CONFIGURATION_TYPE)
        set_target_properties(${ARGV0} PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY_${CMAKE_CONFIGURATION_TYPE} ${DEPLOY_DIR}
            LIBRARY_OUTPUT_DIRECTORY_${CMAKE_CONFIGURATION_TYPE} ${DEPLOY_DIR}
            VS_DEBUGGER_WORKING_DIRECTORY ${DEPLOY_DIR}
            DEBUG_POSTFIX "d"
        )
    endforeach(CMAKE_CONFIGURATION_TYPE)
endfunction()

function(_fastcg_add_library)
    add_library(${ARGN})
    _fastcg_set_target_properties(${ARGV0})
    _fastcg_add_asset_targets(${ARGV0})
endfunction()

function(_fastcg_add_apk_targets)
    set(SRC_GRADLE_PROJECT_DIR "${CMAKE_SOURCE_DIR}/resources/Android/gradle/project")
    set(DST_GRADLE_PROJECT_DIR "${CMAKE_BINARY_DIR}/gradle/${ARGV0}")

    file(GLOB_RECURSE SRC_GRADLE_PROJECT_FILES "${SRC_GRADLE_PROJECT_DIR}/*.*")
    foreach(SRC_GRADLE_PROJECT_FILE IN LISTS SRC_GRADLE_PROJECT_FILES)
        file(RELATIVE_PATH REL_GRADLE_PROJECT_FILE ${SRC_GRADLE_PROJECT_DIR} ${SRC_GRADLE_PROJECT_FILE})
        set(DST_GRADLE_PROJECT_FILE "${DST_GRADLE_PROJECT_DIR}/${REL_GRADLE_PROJECT_FILE}")
        if(DST_GRADLE_PROJECT_FILE MATCHES "\\.template$")
            string(REGEX REPLACE "\\.template$" "" DST_GRADLE_PROJECT_FILE "${DST_GRADLE_PROJECT_FILE}")
            add_custom_command(
                OUTPUT ${DST_GRADLE_PROJECT_FILE}
                COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/cmake/fastcg_template_engine.cmake "${SRC_GRADLE_PROJECT_FILE}" "${DST_GRADLE_PROJECT_FILE}" "project_name=${ARGV0};debuggable=$<IF:$<CONFIG:Debug>,true,false>"
                DEPENDS ${SRC_GRADLE_PROJECT_FILE}
            )
        else()
            add_custom_command(
                OUTPUT ${DST_GRADLE_PROJECT_FILE}
                COMMAND ${CMAKE_COMMAND} -E copy ${SRC_GRADLE_PROJECT_FILE} ${DST_GRADLE_PROJECT_FILE}
                DEPENDS ${SRC_GRADLE_PROJECT_FILE}
            )
        endif()
        list(APPEND DST_GRADLE_RESOURCE_FILES ${DST_GRADLE_PROJECT_FILE})
    endforeach(SRC_GRADLE_PROJECT_FILE)

    if(DST_GRADLE_RESOURCE_FILES)
        add_custom_target(
            ${ARGV0}_PREPARE_GRADLE_PROJECT
            DEPENDS ${ARGV0} ${DST_GRADLE_RESOURCE_FILES}
        )
    else()
        add_custom_target(
            ${ARGV0}_PREPARE_GRADLE_PROJECT
            DEPENDS ${ARGV0}
        )
    endif()

    set(FASTCG_ASSETS_DIR "${FASTCG_DEPLOY}/assets/FastCG")
    set(ASSETS_DIR "${FASTCG_DEPLOY}/assets/${ARGV0}")
    file(TO_CMAKE_PATH "$ENV{NDKROOT}" NDKROOT_PATH)

    add_custom_command(
        TARGET ${ARGV0}_PREPARE_GRADLE_PROJECT PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${DST_GRADLE_PROJECT_DIR}/app/src/main/assets/FastCG
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${FASTCG_ASSETS_DIR} ${DST_GRADLE_PROJECT_DIR}/app/src/main/assets/FastCG
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${DST_GRADLE_PROJECT_DIR}/app/src/main/assets/${ARGV0}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_DIR} ${DST_GRADLE_PROJECT_DIR}/app/src/main/assets/${ARGV0}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/resources/logo.png ${DST_GRADLE_PROJECT_DIR}/app/src/main/res/drawable/logo.png
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${ARGV0}> ${DST_GRADLE_PROJECT_DIR}/app/src/main/jniLibs/arm64-v8a/lib${ARGV0}.so
    )

    add_custom_target(
        ${ARGV0}_BUILD_APK
        WORKING_DIRECTORY ${DST_GRADLE_PROJECT_DIR}
        COMMAND gradlew assemble$<CONFIG>
        BYPRODUCTS ${DST_GRADLE_PROJECT_DIR}/app/build/outputs/apk/$<LOWER_CASE:$<CONFIG>>/${ARGV0}-$<LOWER_CASE:$<CONFIG>>.apk
        DEPENDS ${ARGV0}_PREPARE_GRADLE_PROJECT
    )

    add_custom_target(
        ${ARGV0}_DEPLOY_APK
        COMMAND ${FASTCG_ADB} shell am force-stop com.fastcg.${ARGV0}
        COMMAND ${CMAKE_SOURCE_DIR}/scripts/shell_wrapper "${FASTCG_ADB} uninstall com.fastcg.${ARGV0}"
        COMMAND ${FASTCG_ADB} install ${DST_GRADLE_PROJECT_DIR}/app/build/outputs/apk/$<LOWER_CASE:$<CONFIG>>/${ARGV0}-$<LOWER_CASE:$<CONFIG>>.apk
        DEPENDS ${ARGV0}_BUILD_APK
    )

    add_custom_target(
        ${ARGV0}_LAUNCH_APK
        COMMAND ${FASTCG_ADB} shell am start com.fastcg.${ARGV0}/com.fastcg.MainActivity
        DEPENDS ${ARGV0}_DEPLOY_APK
    )
endfunction()

function(_fastcg_add_android_executable)
    set(REMAINING_ARGS ${ARGN})
    list(REMOVE_AT REMAINING_ARGS 0)
    # turn executables into shared libraries
    add_library(${ARGV0} SHARED ${REMAINING_ARGS})

    _fastcg_add_apk_targets(${ARGV0})
endfunction()

function(_fastcg_add_generated_sources_targets)
    set(SRC_GENERATED_SOURCES_DIR "${CMAKE_SOURCE_DIR}/resources/generated_sources")
    set(DST_GENERATED_SOURCES_DIR "${CMAKE_BINARY_DIR}/generated_sources/${ARGV0}")

    set(SRC_CONFIG_HEADER ${SRC_GENERATED_SOURCES_DIR}/config/Config.h)
    set(SRC_CONFIG_SOURCE ${SRC_GENERATED_SOURCES_DIR}/config/Config.cpp.template)
    set(DST_CONFIG_HEADER ${DST_GENERATED_SOURCES_DIR}/config/Config.h)
    set(DST_CONFIG_SOURCE ${DST_GENERATED_SOURCES_DIR}/config/Config.cpp)

    if(NOT DEFINED ${ARGV0}_MAJOR_VERSION)
        set(MAJOR_VERSION 1)
    else()
        set(MAJOR_VERSION ${ARGV0}_MAJOR_VERSION)
    endif()
    if(NOT DEFINED ${ARGV0}_MINOR_VERSION)
        set(MINOR_VERSION 0)
    else()
        set(MINOR_VERSION ${ARGV0}_MINOR_VERSION)
    endif()
    if(NOT DEFINED ${ARGV0}_PATCH_VERSION)
        set(PATCH_VERSION 0)
    else()
        set(PATCH_VERSION ${ARGV0}_PATCH_VERSION)
    endif()

    set(${ARGV0}_MAJOR_VERSION ${MAJOR_VERSION} CACHE STRING "${ARGV0} major version")
    set(${ARGV0}_MINOR_VERSION ${MINOR_VERSION} CACHE STRING "${ARGV0} minor version")
    set(${ARGV0}_PATCH_VERSION ${PATCH_VERSION} CACHE STRING "${ARGV0} patch version")

    add_custom_command(
        OUTPUT ${DST_CONFIG_HEADER}
        COMMAND ${CMAKE_COMMAND} -E copy ${SRC_CONFIG_HEADER} ${DST_CONFIG_HEADER}
        DEPENDS ${SRC_CONFIG_HEADER}
    )
    set(PARAMS "project_name=${ARGV0};major_version=${${ARGV0}_MAJOR_VERSION};minor_version=${${ARGV0}_MINOR_VERSION};patch_version=${${ARGV0}_PATCH_VERSION}")
    add_custom_command(
        OUTPUT ${DST_CONFIG_SOURCE}
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/cmake/fastcg_template_engine.cmake "${SRC_CONFIG_SOURCE}" "${DST_CONFIG_SOURCE}" ${PARAMS}
        DEPENDS ${SRC_CONFIG_SOURCE}
    )

    list(APPEND DST_GENERATED_SOURCES ${DST_CONFIG_SOURCE} ${DST_CONFIG_HEADER})

    add_custom_target(
        ${ARGV0}_GENERATE_SOURCES
        DEPENDS ${DST_GENERATED_SOURCES}
    )
    target_sources(${ARGV0} PRIVATE ${DST_GENERATED_SOURCES})
    target_include_directories(${ARGV0} PRIVATE ${DST_GENERATED_SOURCES_DIR})
    add_dependencies(${ARGV0} ${ARGV0}_GENERATE_SOURCES)
endfunction()

function(fastcg_add_executable)
    if(FASTCG_PLATFORM STREQUAL "Android")
        _fastcg_add_android_executable(${ARGN})
    else()
        add_executable(${ARGN})
    endif()
    target_compile_definitions(${ARGV0} PUBLIC FASTCG_EXECUTABLE_NAME="${ARGV0}")
    _fastcg_target_compile_definitions(${ARGV0})
    _fastcg_target_compile_options(${ARGV0})
    _fastcg_set_target_properties(${ARGV0})
    target_link_libraries(${ARGV0} FastCG)
    _fastcg_add_asset_targets(${ARGV0})
    _fastcg_add_generated_sources_targets(${ARGV0})
endfunction()

function(fastcg_add_library)
    _fastcg_add_library(${ARGN})
    _fastcg_target_compile_definitions(${ARGV0})
    _fastcg_target_compile_options(${ARGV0})
endfunction()

function(fastcg_add_tool)
    if(NOT FASTCG_PLATFORM STREQUAL "Android")
        add_executable(${ARGN})
        _fastcg_set_target_properties(${ARGV0})
    endif()
endfunction()