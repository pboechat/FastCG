set(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_C_COMPILER clang CACHE FILEPATH "" FORCE)
set(CMAKE_CXX_COMPILER clang++ CACHE FILEPATH "" FORCE)

find_program(LLVM_AR NAMES llvm-ar)
if(LLVM_AR)
    set(CMAKE_AR "${LLVM_AR}" CACHE FILEPATH "" FORCE)
else()
    set(CMAKE_AR ar CACHE FILEPATH "" FORCE)
endif()

find_program(LLVM_RANLIB NAMES llvm-ranlib)
if(LLVM_RANLIB)
    set(CMAKE_RANLIB "${LLVM_RANLIB}" CACHE FILEPATH "" FORCE)
else()
    set(CMAKE_RANLIB ranlib CACHE FILEPATH "" FORCE)
endif()

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_program(LLD_LINKER NAMES ld.lld lld)
option(USE_LLD "Use LLVM lld linker if found" OFF)
if(USE_LLD AND LLD_LINKER)
    add_link_options(-fuse-ld=lld)
endif()

option(USE_LIBCXX "Use libc++ (Clang C++ standard library) instead of libstdc++" OFF)
if(USE_LIBCXX)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-stdlib=libc++>)
    add_link_options(-stdlib=libc++)
endif()

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

set(ENABLE_LTO OFF CACHE BOOL "Enable interprocedural optimization (LTO)")
if(ENABLE_LTO)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT lto_supported OUTPUT lto_msg)
  if(lto_supported)
      set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
  endif()
endif()

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)
