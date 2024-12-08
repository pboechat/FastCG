cmake_minimum_required(VERSION 3.10)

# Usage: 
# cmake -P fastcg_glsl_processor.cmake "platform" "path/to/input" "path/to/output" "glsl_glsl_version"

set(platform ${CMAKE_ARGV3})
set(input_file ${CMAKE_ARGV4})
set(output_file ${CMAKE_ARGV5})
set(glsl_version ${CMAKE_ARGV6})

if(platform STREQUAL "" OR input_file STREQUAL "" OR output_file STREQUAL "" OR glsl_version STREQUAL "")
    message(FATAL_ERROR "You must provide platform, input_file, output_file and glsl_version")
endif()

file(READ ${input_file} file_content)

set(glsl_prefix "#version ${glsl_version}")
set(glsl_prefix "${glsl_prefix}\n#extension GL_GOOGLE_include_directive : enable")
if(input_file MATCHES "\\.frag$")
    set(glsl_prefix "${glsl_prefix}\n#define FASTCG_FRAGMENT_SHADER")
elseif(input_file MATCHES "\\.vert$")
    set(glsl_prefix "${glsl_prefix}\n#define FASTCG_VERTEX_SHADER")
elseif(input_file MATCHES "\\.comp$")
    set(glsl_prefix "${glsl_prefix}\n#define FASTCG_COMPUTE_SHADER")
endif()
if(platform STREQUAL "Android") 
    if(input_file MATCHES "\\.frag$")
        set(glsl_prefix "${glsl_prefix}\nprecision mediump float;")
    endif()
endif()


file(WRITE ${output_file} "${glsl_prefix}\n\n${file_content}")

message(STATUS "File prepending completed for: ${output_file}")