# PLATFORM

if (DEFINED FASTCG_PLATFORM)
    unset(FASTCG_PLATFORM)
endif()
if(WIN32)
    set(FASTCG_PLATFORM "Windows" CACHE STRING "FastCG platform")
elseif(UNIX AND NOT APPLE)
    set(FASTCG_PLATFORM "Linux" CACHE STRING "FastCG platform")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_definitions(-D_DEBUG=1)
    endif()
endif()
message("FastCG platform: ${FASTCG_PLATFORM}")

# GRAPHICS SYSTEM

if (DEFINED FASTCG_GRAPHICS_SYSTEM)
    set(FASTCG_GRAPHICS_SYSTEM_VALUE ${FASTCG_GRAPHICS_SYSTEM})
    unset(FASTCG_GRAPHICS_SYSTEM)
else()
    set(FASTCG_GRAPHICS_SYSTEM_VALUE "OpenGL")
endif()
set(FASTCG_GRAPHICS_SYSTEM ${FASTCG_GRAPHICS_SYSTEM_VALUE} CACHE STRING "FastCG graphics system")
message("FastCG graphics system: ${FASTCG_GRAPHICS_SYSTEM}")

# DEPLOY DIRECTORY

if (DEFINED FASTCG_DEPLOY)
    set(FASTCG_DEPLOY_VALUE ${FASTCG_DEPLOY})
    unset(FASTCG_DEPLOY)
else()
    set(FASTCG_DEPLOY_VALUE ${CMAKE_SOURCE_DIR}/deploy)
endif()
set(FASTCG_DEPLOY ${FASTCG_DEPLOY_VALUE} CACHE STRING "FastCG deploy directory")
message("FastCG deploy directory: ${FASTCG_DEPLOY}")
set_property(DIRECTORY PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${FASTCG_DEPLOY})

# BUILD SAMPLES

if (DEFINED FASTCG_BUILD_SAMPLES)
    set(FASTCG_BUILD_SAMPLES_VALUE ${FASTCG_BUILD_SAMPLES})
    unset(FASTCG_BUILD_SAMPLES)
else()
    set(FASTCG_BUILD_SAMPLES_VALUE true)
endif()
set(FASTCG_BUILD_SAMPLES ${FASTCG_BUILD_SAMPLES_VALUE} CACHE BOOL "Build FastCG samples")