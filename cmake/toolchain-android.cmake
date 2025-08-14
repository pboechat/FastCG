set(ANDROID_ABI "arm64-v8a")
set(ANDROID_PLATFORM "android-33")
set(ANDROID_STL "c++_static")
set(ANDROID_TOOLCHAIN "clang")

if (NOT DEFINED ENV{NDKROOT})
    message(FATAL_ERROR "NDKROOT environment variable is not set. Please set it to the path of your Android NDK.")
endif()

message(STATUS "Using NDKROOT=$ENV{NDKROOT}")

if (NOT EXISTS "$ENV{NDKROOT}/build/cmake/android.toolchain.cmake")
    message(FATAL_ERROR "The Android toolchain file does not exist at $ENV{NDKROOT}/build/cmake/android.toolchain.cmake. Please check your NDK installation.")
endif()

include("$ENV{NDKROOT}/build/cmake/android.toolchain.cmake")

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_FLAGS_DEBUG_INIT "-O0 -g3")
set(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "-O2 -g -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-O0 -g3")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O2 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")