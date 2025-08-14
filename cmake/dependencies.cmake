cmake_minimum_required(VERSION 3.20)

set(FASTCG_DEPS_CACHE  "${CMAKE_SOURCE_DIR}/.deps" CACHE PATH "Shared third-party cache")

include(FetchContent)

function(_fastcg_fetch_content_declare name)
    set(options)
    set(oneValueArgs GIT_REPOSITORY GIT_TAG URL)
    set(multiValueArgs GIT_SUBMODULES PATCH_COMMAND FIND_PACKAGE_ARGS)
    cmake_parse_arguments(F "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    file(TO_CMAKE_PATH "${FASTCG_DEPS_CACHE}" FASTCG_DEPS_CACHE_NORM)

    set(SOURCE "${FASTCG_DEPS_CACHE_NORM}/${name}")
    set(BIN "${CMAKE_BINARY_DIR}/_deps/${name}-build")

    string(TOUPPER "${name}" NAME_UP)
    if(DEFINED FETCHCONTENT_SOURCE_DIR_${NAME_UP})
        set(SOURCE "${FETCHCONTENT_SOURCE_DIR_${NAME_UP}}")
    endif()
    if(DEFINED FETCHCONTENT_BINARY_DIR_${NAME_UP})
        set(BIN "${FETCHCONTENT_BINARY_DIR_${NAME_UP}}")
    endif()

    set(HAS_SOURCE FALSE)
    if(EXISTS "${SOURCE}/.git")
        set(HAS_SOURCE TRUE)
    else()
        file(GLOB HAS_ANY_SOURCE "${SOURCE}/*")
        if(HAS_ANY_SOURCE)
            set(HAS_SOURCE TRUE)
        endif()
    endif()

    set(F_ARGS SOURCE_DIR "${SOURCE}" BINARY_DIR "${BIN}")
    if(NOT HAS_SOURCE)
        get_filename_component(SOURCE_PARENT "${SOURCE}" DIRECTORY)
        if(SOURCE_PARENT)
            file(MAKE_DIRECTORY "${SOURCE_PARENT}")
        endif()

        if(F_GIT_REPOSITORY)
            list(APPEND F_ARGS GIT_REPOSITORY "${F_GIT_REPOSITORY}")
        endif()
        if(F_GIT_TAG)
            list(APPEND F_ARGS GIT_TAG "${F_GIT_TAG}")
        endif()
        if(F_URL)
            list(APPEND F_ARGS URL "${F_URL}")
        endif()
        list(APPEND F_ARGS GIT_SHALLOW TRUE)
        if(F_GIT_SUBMODULES)
            list(APPEND F_ARGS GIT_SUBMODULES ${F_GIT_SUBMODULES})
        endif()
    else()
        set(FETCHCONTENT_SOURCE_DIR_${NAME_UP} "${SOURCE}" CACHE PATH "" FORCE)
    endif()

    if(F_PATCH_COMMAND)
        list(APPEND F_ARGS PATCH_COMMAND ${F_PATCH_COMMAND})
    endif()
    if(F_FIND_PACKAGE_ARGS)
        list(APPEND F_ARGS FIND_PACKAGE_ARGS ${F_FIND_PACKAGE_ARGS})
    endif()

    set(${name}_OVERRIDE_SOURCE_DIR "${SOURCE}" PARENT_SCOPE)
    set(${name}_OVERRIDE_BINARY_DIR "${BIN}" PARENT_SCOPE)

    FetchContent_Declare(${name} ${F_ARGS})
endfunction()

set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_FULLY_DISCONNECTED ON CACHE BOOL "")

if(FASTCG_PLATFORM STREQUAL "Linux")
    find_package(X11 REQUIRED COMPONENTS Xext Xrender)
endif()

if(FASTCG_GRAPHICS_SYSTEM STREQUAL "OpenGL")
    if(FASTCG_PLATFORM STREQUAL "Android")
        find_library(EGL_LIB NAMES EGL)
        find_library(GLES_LIB NAMES GLESv3 GLESv2)
    else()
        find_package(OpenGL REQUIRED)
    endif()
elseif(FASTCG_GRAPHICS_SYSTEM STREQUAL "Vulkan")
    find_package(Vulkan REQUIRED)
endif()

if(FASTCG_PLATFORM STREQUAL "Android")
    set(_NATIVE_APP_GLUE_DIR "$ENV{NDKROOT}/sources/android/native_app_glue")
    if(DEFINED ANDROID_NATIVE_APP_GLUE_DIR)
        set(_NATIVE_APP_GLUE_DIR "${ANDROID_NATIVE_APP_GLUE_DIR}")
    endif()
    file(TO_CMAKE_PATH "${_NATIVE_APP_GLUE_DIR}" _NATIVE_APP_GLUE_DIR)
    if(EXISTS "${_NATIVE_APP_GLUE_DIR}/android_native_app_glue.c")
        # -------- android_native_app_glue --------
        add_library(android_native_app_glue STATIC
            "${_NATIVE_APP_GLUE_DIR}/android_native_app_glue.c"
        )
        target_include_directories(android_native_app_glue PUBLIC 
            "${_NATIVE_APP_GLUE_DIR}"
        )
        message(STATUS "Using NDK native_app_glue at: ${_NATIVE_APP_GLUE_DIR}")
    else()
        message(FATAL_ERROR
            "native_app_glue not found. Set ANDROID_NATIVE_APP_GLUE_DIR or NDKROOT."
        )
    endif()
endif()

if(FASTCG_GRAPHICS_SYSTEM STREQUAL "OpenGL" AND NOT FASTCG_PLATFORM STREQUAL "Android")
    # -------- GLEW --------
    _fastcg_fetch_content_declare(glew_cmake
        GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
        GIT_TAG 918ece3da858c2e28e10f6507378af01647cb139
        GIT_SHALLOW TRUE
    )
    set(USE_GLU ON CACHE BOOL "" FORCE)
    set(ONLY_LIBS ON CACHE BOOL "" FORCE)
    set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glew_cmake)
endif()

# -------- glm --------
_fastcg_fetch_content_declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 2d4c4b4dd31fde06cfffad7915c2b3006402322f
    GIT_SHALLOW TRUE
)
set(GLM_ENABLE_CXX_20 ON CACHE BOOL "" FORCE)
FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
    FetchContent_Populate(glm)
endif()
if(NOT TARGET glm)
    add_library(glm INTERFACE)
    target_include_directories(glm INTERFACE 
        "$<BUILD_INTERFACE:${glm_SOURCE_DIR}>"
    )
    add_library(glm::glm ALIAS glm)
endif()
target_compile_definitions(glm INTERFACE 
    GLM_ENABLE_EXPERIMENTAL
)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(glm INTERFACE  
        /wd4756 # overflow in constant arithmetic
    )
endif()

# -------- imgui --------
_fastcg_fetch_content_declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG 2b24f5fa71d124de108da54d9da49ea3049dc91e
    GIT_SHALLOW TRUE
)
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
endif()
if(NOT TARGET imgui)
    add_library(imgui STATIC 
        "${imgui_SOURCE_DIR}/imgui.cpp"
        "${imgui_SOURCE_DIR}/imgui_draw.cpp"
        "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
        "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    )
    target_include_directories(imgui PUBLIC
        "${imgui_SOURCE_DIR}"
    )
    add_library(imgui::imgui ALIAS imgui)
endif()
target_compile_definitions(imgui PUBLIC 
    "-DImDrawIdx=unsigned int"
)

# -------- ImGuizmo --------
_fastcg_fetch_content_declare(ImGuizmo
    GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
    GIT_TAG 3e725628fa64986c71ebc7c764621a8e7423fef4
    GIT_SHALLOW TRUE
)
FetchContent_GetProperties(ImGuizmo)
if(NOT ImGuizmo_POPULATED)
    FetchContent_Populate(ImGuizmo)
endif()
if(NOT TARGET ImGuizmo)
    add_library(ImGuizmo STATIC
        "${imguizmo_SOURCE_DIR}/GraphEditor.cpp"
        "${imguizmo_SOURCE_DIR}/ImCurveEdit.cpp"
        "${imguizmo_SOURCE_DIR}/ImGradient.cpp"
        "${imguizmo_SOURCE_DIR}/ImGuizmo.cpp"
        "${imguizmo_SOURCE_DIR}/ImSequencer.cpp"
    )
    target_include_directories(ImGuizmo PUBLIC
        "${imguizmo_SOURCE_DIR}"
        "${imgui_SOURCE_DIR}"
    )
    target_link_libraries(ImGuizmo PUBLIC 
        imgui
    )
    add_library(ImGuizmo::ImGuizmo ALIAS ImGuizmo)
endif()

# -------- ImGuiFileDialog --------
_fastcg_fetch_content_declare(ImGuiFileDialog
    GIT_REPOSITORY https://github.com/aiekick/ImGuiFileDialog.git
    GIT_TAG 58292701b8224e58703d15811b69a39444185e30
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(ImGuiFileDialog)
if(TARGET ImGuiFileDialog)
    target_link_libraries(ImGuiFileDialog PUBLIC imgui)
    if(DEFINED imgui_SOURCE_DIR)
        target_include_directories(ImGuiFileDialog PUBLIC 
            "${imgui_SOURCE_DIR}"
        )
    endif()
endif()

# -------- KTX-Software --------
_fastcg_fetch_content_declare(ktx
    GIT_REPOSITORY https://github.com/KhronosGroup/KTX-Software.git
    GIT_TAG beef80159525d9fb7abb8645ea85f4c4f6842e8f
    GIT_SHALLOW TRUE
)
set(KTX_FEATURE_TOOLS OFF CACHE BOOL "" FORCE)
set(KTX_FEATURE_STATIC_LIBRARY ON CACHE BOOL "" FORCE)
set(KTX_FEATURE_TESTS OFF CACHE BOOL "" FORCE)
set(KTX_FEATURE_GL_UPLOAD OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(ktx)

# -------- messagebox-x11 --------
if(FASTCG_PLATFORM STREQUAL "Linux")
    _fastcg_fetch_content_declare(messagebox_x11
        GIT_REPOSITORY https://github.com/Eleobert/MessageBox-X11.git
        GIT_TAG 0fc63f4159282bb6c9d94a3bdcdbac209b576efd
        GIT_SHALLOW TRUE
    )
    FetchContent_GetProperties(messagebox_x11)
    if(NOT messagebox_x11_POPULATED)
        FetchContent_Populate(messagebox_x11)
    endif()
    if(NOT TARGET messagebox_x11)
        add_library(messagebox_x11 STATIC
            "${messagebox_x11_SOURCE_DIR}/messagebox.c"
        )
        target_include_directories(messagebox_x11 PUBLIC
            "${messagebox_x11_SOURCE_DIR}"
        )
        add_library(messagebox_x11::messagebox_x11 ALIAS messagebox_x11)
        target_compile_definitions(messagebox_x11 PRIVATE 
            _POSIX_C_SOURCE=200809L
        )
    endif()
endif()

# -------- rapidjson --------
_fastcg_fetch_content_declare(rapidjson
    GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
    GIT_TAG 24b5e7a8b27f42fa16b96fc70aade9106cf7102f
    GIT_SHALLOW TRUE
)
set(RAPIDJSON_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_GetProperties(rapidjson)
if(NOT rapidjson_POPULATED)
    FetchContent_Populate(rapidjson)
endif()
if(NOT TARGET rapidjson)
    add_library(rapidjson INTERFACE)
    target_include_directories(rapidjson INTERFACE
        "$<BUILD_INTERFACE:${rapidjson_SOURCE_DIR}/include>"
    )
    add_library(rapidjson::rapidjson ALIAS rapidjson)
endif()

# -------- stb --------
_fastcg_fetch_content_declare(stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG f58f558c120e9b32c217290b80bad1a0729fbb2c
    GIT_SHALLOW TRUE
)
FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
    FetchContent_Populate(stb)
endif()
if(NOT TARGET stb)
    add_library(stb INTERFACE)
    target_include_directories(stb INTERFACE
        "$<BUILD_INTERFACE:${stb_SOURCE_DIR}>"
    )
    add_library(stb::stb ALIAS stb)
endif()

# -------- tinyobj_loader_c --------
_fastcg_fetch_content_declare(tinyobj_loader_c
    GIT_REPOSITORY https://github.com/syoyo/tinyobjloader-c.git
    GIT_TAG 0f8ea84a0da616790b476912e1ccc641f36b463e
    GIT_SHALLOW TRUE
)
FetchContent_GetProperties(tinyobj_loader_c)
if(NOT tinyobj_loader_c_POPULATED)
    FetchContent_Populate(tinyobj_loader_c)
endif()
if(NOT TARGET tinyobj_loader_c)
    add_library(tinyobj_loader_c INTERFACE)
    target_include_directories(tinyobj_loader_c INTERFACE
        "$<BUILD_INTERFACE:${tinyobj_loader_c_SOURCE_DIR}>"
    )
    add_library(tinyobj_loader_c::tinyobj_loader_c ALIAS tinyobj_loader_c)
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(tinyobj_loader_c INTERFACE  
        /wd4706 # assignment within conditional expression
    )
endif()

if(FASTCG_GRAPHICS_SYSTEM STREQUAL "Vulkan")
    if(NOT FASTCG_PLATFORM STREQUAL "Linux")
        # -------- SPIRV-Cross --------
        _fastcg_fetch_content_declare(SPIRV_Cross
            GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross.git
            GIT_TAG c9965407c3cca05cafc350efe3b8fd86badd968f
            GIT_SHALLOW TRUE
        )
        set(SPIRV_CROSS_CLI OFF CACHE BOOL "" FORCE)
        set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
        set(SPIRV_CROSS_ENABLE_GLSL ON CACHE BOOL "" FORCE)
        set(SPIRV_CROSS_ENABLE_HLSL ON CACHE BOOL "" FORCE)
        set(SPIRV_CROSS_ENABLE_MSL ON CACHE BOOL "" FORCE)
        set(SPIRV_CROSS_ENABLE_CPP OFF CACHE BOOL "" FORCE)
        set(SPIRV_CROSS_ENABLE_REFLECT ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(SPIRV_Cross)
    endif()

    # -------- VulkanMemoryAllocator --------
    _fastcg_fetch_content_declare(VulkanMemoryAllocator
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        GIT_TAG 1076b348abd17859a116f4b111c43d58a588a086
        GIT_SHALLOW TRUE
    )
    set(VMA_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(VulkanMemoryAllocator)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(VulkanMemoryAllocator INTERFACE 
            /wd4100 # unreferenced formal parameter
            /wd4189 # local variable is initialized but not referenced
            /wd4324 # structure was padded due to alignment specifier
            /wd4456 # declaration of 'x' hides previous local declaration
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(VulkanMemoryAllocator INTERFACE 
            -Wno-gnu-zero-variadic-macro-arguments
            -Wno-nested-anon-types
            -Wno-missing-braces
            -Wno-nullability-completeness
            -Wno-nullability-extension
            -Wno-unused-function
            -Wno-unused-private-field
            -Wno-unused-variable
        )
    endif()
endif()

# -------- zlib --------
_fastcg_fetch_content_declare(zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG 5a82f71ed1dfc0bec044d9702463dbdf84ea3b71
    GIT_SHALLOW TRUE
)
set(ZLIB_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(zlib)
