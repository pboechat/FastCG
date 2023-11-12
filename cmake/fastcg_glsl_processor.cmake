cmake_minimum_required(VERSION 3.10)

# Usage: 
# cmake -P fastcg_glsl_processor.cmake "path/to/input" "path/to/output" "glsl_glsl_version"

set(input_file ${CMAKE_ARGV3} CACHE FILEPATH "Path to the input file")
set(output_file ${CMAKE_ARGV4} CACHE FILEPATH "Path to the output file")
set(glsl_version ${CMAKE_ARGV5} CACHE FILEPATH "GLSL version")

if(input_file STREQUAL "" OR output_file STREQUAL "" OR glsl_version STREQUAL "")
    message(FATAL_ERROR "You must provide input_file, output_file and glsl_version")
endif()

file(READ ${input_file} file_content)

set(glsl_prefix "#version ${glsl_version}")
set(glsl_prefix "${glsl_prefix}\n#extension GL_GOOGLE_include_directive : enable")
if(input_file MATCHES "\\.frag$")
    set(glsl_prefix "${glsl_prefix}\nprecision mediump float;")
endif()


file(WRITE ${output_file} "${glsl_prefix}\n\n${file_content}")

message(STATUS "File prepending completed for: ${output_file}")