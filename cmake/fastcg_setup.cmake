# Declare platform variable

if (DEFINED FASTCG_PLATFORM)
    unset(FASTCG_PLATFORM)
endif()
if(WIN32)
    set(FASTCG_PLATFORM "Windows" CACHE STRING "FastCG platform")
elseif(UNIX AND NOT APPLE)
    set(FASTCG_PLATFORM "Linux" CACHE STRING "FastCG platform")
endif()
message("FastCG platform: ${FASTCG_PLATFORM}")

# Declare graphics system variable

set(FASTCG_GRAPHICS_SYSTEM "OpenGL" CACHE STRING "FastCG graphics system")
message("FastCG graphics system: ${FASTCG_GRAPHICS_SYSTEM}")
set_property(CACHE FASTCG_GRAPHICS_SYSTEM PROPERTY STRINGS OpenGL Vulkan)

# Declare deploy variable

if (DEFINED FASTCG_DEPLOY)
    set(FASTCG_DEPLOY_VALUE ${FASTCG_DEPLOY})
    unset(FASTCG_DEPLOY)
else()
    set(FASTCG_DEPLOY_VALUE ${CMAKE_SOURCE_DIR}/deploy)
endif()
set(FASTCG_DEPLOY ${FASTCG_DEPLOY_VALUE} CACHE STRING "FastCG deploy directory")
message("FastCG deploy directory: ${FASTCG_DEPLOY}")
set_property(DIRECTORY PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${FASTCG_DEPLOY})

# Declare options

option(FASTCG_BUILD_SAMPLES "Build FastCG samples" ON)
option(FASTCG_USE_TEXT_SHADERS "Use text shaders" OFF)
option(FASTCG_DISABLE_GPU_TIMING "Disable GPU timing" OFF)
option(FASTCG_DISABLE_GPU_VALIDATION "Disable GPU validation" OFF)

# Find necessary programs

if (NOT FASTCG_USE_TEXT_SHADERS)
    find_program(FASTCG_GLSLANGVALIDATOR glslangValidator DOC "Path to the glslangValidator executable")

    if (FASTCG_GLSLANGVALIDATOR)
    message(STATUS "Found glslangValidator: ${FASTCG_GLSLANGVALIDATOR}")
    else()
    message(FATAL_ERROR "Could not find the glslangValidator executable!")
    endif()
endif()