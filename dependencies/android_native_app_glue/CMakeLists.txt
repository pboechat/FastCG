cmake_minimum_required(VERSION 3.20)

project(android_native_app_glue)

file(TO_CMAKE_PATH "$ENV{NDKROOT}" NDKROOT_PATH)
set(SOURCES ${NDKROOT_PATH}/sources/android/native_app_glue/android_native_app_glue.c)
set(HEADERS ${NDKROOT_PATH}/sources/android/native_app_glue/android_native_app_glue.h)

set(${PROJECT_NAME}_INCLUDE_DIRS ${NDKROOT_PATH}/sources/android/native_app_glue CACHE INTERNAL "${PROJECT_NAME}: includes" FORCE)

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

source_group("includes" FILES ${HEADERS})
source_group("src" FILES ${SOURCES})

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
