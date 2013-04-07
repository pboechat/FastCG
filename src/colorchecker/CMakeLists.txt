cmake_minimum_required(VERSION 2.8)

project(ColorChecker)

# --------------------------------------------------
#	OpenGL
# --------------------------------------------------

find_package(OpenGL REQUIRED)

include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)

# --------------------------------------------------
#	Other dependencies
# --------------------------------------------------

set(FASTCGCORE_DIR ${CMAKE_SOURCE_DIR}/src/core)
set(GLM_DIR ${CMAKE_SOURCE_DIR}/dependencies/glm)
set(LOG4CPP_DIR ${CMAKE_SOURCE_DIR}/dependencies/log4cpp)

set(FASTCGCORE_INCLUDES ${FASTCGCORE_DIR}/include)
set(GLM_INCLUDES ${GLM_DIR}/include)
set(LOG4CPP_INCLUDES ${LOG4CPP_DIR}/include)

# --------------------------------------------------
#	Project definitions
# --------------------------------------------------

file(GLOB SOURCE_FILES *.cpp)
file(GLOB HEADER_FILES *.h)

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(BASE_FOLDER_PATH ${CMAKE_SOURCE_DIR})
set(DATA_FOLDER_PATH ${CMAKE_SOURCE_DIR}/data)
set(TESTS_DATA_FOLDER_PATH ${CMAKE_SOURCE_DIR}/data/tests)

configure_file (
  "${CMAKE_SOURCE_DIR}/src/colorchecker/Config.cpp.in"
  "${CMAKE_SOURCE_DIR}/src/colorchecker/Config.cpp"
)

include_directories(
	${FASTCGCORE_INCLUDES}
    ${GLM_INCLUDES}
    ${LOG4CPP_INCLUDES}
)

add_executable(
    ColorChecker
    ${CMAKE_SOURCE_DIR}/src/colorchecker/Config.cpp
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

target_link_libraries(
    ColorChecker
	FastCGCore
    ${OPENGL_LIBRARIES}
    winmm.lib
)

set_target_properties(ColorChecker PROPERTIES DEBUG_POSTFIX d)