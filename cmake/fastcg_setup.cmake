# Setup platform

set(FASTCG_PLATFORM ${CMAKE_SYSTEM_NAME})
set(FASTCG_PLATFORM ${FASTCG_PLATFORM} CACHE STRING "FastCG platform")
set(FASTCG_PLATFORMS "Windows" "Linux" "Android")
set_property(CACHE FASTCG_PLATFORM PROPERTY STRINGS ${FASTCG_PLATFORMS})
if(NOT FASTCG_PLATFORM IN_LIST FASTCG_PLATFORMS)
    message(FATAL_ERROR "Invalid platform: ${FASTCG_PLATFORM}")
endif()
message(STATUS "FastCG platform: ${FASTCG_PLATFORM}")

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    set(FASTCG_IGNORE_RETURN_PREFIX cmd /C)
    set(FASTCG_IGNORE_RETURN_SUFFIX || exit 0)
else()
    set(FASTCG_IGNORE_RETURN_PREFIX sh -c)
    set(FASTCG_IGNORE_RETURN_SUFFIX || true)
endif()

# Setup graphics system

set(FASTCG_GRAPHICS_SYSTEM "OpenGL" CACHE STRING "FastCG graphics system")
set(FASTCG_GRAPHICS_SYSTEMS "OpenGL" "Vulkan")
set_property(CACHE FASTCG_GRAPHICS_SYSTEM PROPERTY STRINGS ${FASTCG_GRAPHICS_SYSTEMS})
if(NOT FASTCG_GRAPHICS_SYSTEM IN_LIST FASTCG_GRAPHICS_SYSTEMS)
    message(FATAL_ERROR "Invalid graphics system: ${FASTCG_GRAPHICS_SYSTEM}")
endif()
message(STATUS "FastCG graphics system: ${FASTCG_GRAPHICS_SYSTEM}")

# Setup deploy directory

if(DEFINED FASTCG_DEPLOY)
    set(FASTCG_DEPLOY_VALUE ${FASTCG_DEPLOY})
    unset(FASTCG_DEPLOY)
else()
    set(FASTCG_DEPLOY_VALUE ${CMAKE_SOURCE_DIR}/deploy/${FASTCG_PLATFORM})
endif()
set(FASTCG_DEPLOY ${FASTCG_DEPLOY_VALUE} CACHE STRING "FastCG deploy directory")
set_property(DIRECTORY PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${FASTCG_DEPLOY})
message(STATUS "FastCG deploy directory: ${FASTCG_DEPLOY}")

# Setup options

option(FASTCG_BUILD_SAMPLES "Build samples" ON)
if(FASTCG_PLATFORM STREQUAL "Android" AND FASTCG_GRAPHICS_SYSTEM STREQUAL "OpenGL")
    # TODO: use precompiled shaders in GLES if possible at all
    set(FASTCG_USE_TEXT_SHADERS ON)
    # time queries require either vendor specific solutions or an extension in GLES
    set(FASTCG_DISABLE_GPU_TIMING ON)
else()
    option(FASTCG_USE_TEXT_SHADERS "Use text shaders" OFF)
    option(FASTCG_DISABLE_GPU_TIMING "Disable GPU timing" OFF)
endif()
option(FASTCG_DISABLE_GPU_VALIDATION "Disable GPU validation" OFF)
option(FASTCG_ENABLE_VERBOSE_LOGGING "Enable verbose logging" OFF)

message(STATUS "FastCG - Build samples = ${FASTCG_BUILD_SAMPLES}")
message(STATUS "FastCG - Use text shaders = ${FASTCG_USE_TEXT_SHADERS}")
message(STATUS "FastCG - Disable GPU timing = ${FASTCG_DISABLE_GPU_TIMING}")
message(STATUS "FastCG - Disable GPU validation = ${FASTCG_DISABLE_GPU_VALIDATION}")
message(STATUS "FastCG - Enable verbose logging = ${FASTCG_ENABLE_VERBOSE_LOGGING}")

# Find necessary programs

if(NOT FASTCG_USE_TEXT_SHADERS)
    find_program(FASTCG_GLSLANGVALIDATOR glslangValidator DOC "Path to the glslangValidator executable")

    if(FASTCG_GLSLANGVALIDATOR)
        message(STATUS "Found glslangValidator: ${FASTCG_GLSLANGVALIDATOR}")
    else()
        message(FATAL_ERROR "Couldn't find the glslangValidator executable!")
    endif()
endif()

if(FASTCG_PLATFORM STREQUAL "Android")
    find_program(FASTCG_ADB adb DOC "Path to the adb executable")
    if(NOT FASTCG_ADB)
        message(FATAL_ERROR "Couldn't find the adb executable!")
    endif()
    message(STATUS "Found adb: ${FASTCG_ADB}")
endif()

# Setup compiler options

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
    add_compile_options(/WX)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    add_compile_options(-Werror)
endif()