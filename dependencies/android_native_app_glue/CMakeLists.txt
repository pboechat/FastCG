cmake_minimum_required(VERSION 3.20)

project(android_native_app_glue)

file(TO_CMAKE_PATH "$ENV{NDKROOT}" NDKROOT_PATH)
set(SOURCES ${NDKROOT_PATH}/sources/android/native_app_glue/android_native_app_glue.c)
set(HEADERS ${NDKROOT_PATH}/sources/android/native_app_glue/android_native_app_glue.h)

source_group("includes" FILES ${HEADERS})
source_group("src" FILES ${SOURCES})

fastcg_add_dependency_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
target_include_directories(${PROJECT_NAME} INTERFACE ${NDKROOT_PATH}/sources/android/native_app_glue)
