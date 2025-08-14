cmake_minimum_required(VERSION 3.20)

file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

set(FASTCG_EXAMPLES)
foreach(child ${children})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child})
        add_subdirectory(${child})
        list(APPEND FASTCG_EXAMPLES ${child})
    endif()
endforeach()

message(STATUS "FastCG examples: ${FASTCG_EXAMPLES}")
