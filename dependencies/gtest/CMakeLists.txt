cmake_minimum_required(VERSION 2.6.2)

option(BUILD_SHARED_LIBS "Build shared libraries (DLLs)." OFF)

option(gtest_force_shared_crt "Use shared (DLL) run-time lib even when Google Test is built as static lib." ON)
option(gtest_disable_pthreads "Disable uses of pthreads in gtest." OFF)

include(cmake/hermetic_build.cmake OPTIONAL)

if (COMMAND pre_project_set_up_hermetic_build)
	pre_project_set_up_hermetic_build()
endif()

project(gtest CXX C)

if (COMMAND set_up_hermetic_build)
	set_up_hermetic_build()
endif()

include(cmake/internal_utils.cmake)

config_compiler_and_linker()

include_directories(
	${gtest_SOURCE_DIR}/include
	${gtest_SOURCE_DIR}
)

link_directories(${gtest_BINARY_DIR}/src)

cxx_library(gtest "${cxx_strict}" src/gtest-all.cc)

#cxx_library(gtest_main "${cxx_strict}" src/gtest_main.cc)
#target_link_libraries(gtest_main gtest)
