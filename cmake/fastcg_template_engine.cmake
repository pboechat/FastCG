cmake_minimum_required(VERSION 3.10)

# Usage: 
# cmake -P fastcg_template_engine.cmake "path/to/template" "path/to/output" "key1=value1;key2=value2;key3=value3"

set(template_file ${CMAKE_ARGV3} CACHE FILEPATH "Path to the template file")
set(output_file ${CMAKE_ARGV4} CACHE FILEPATH "Path to the output file")
set(params ${CMAKE_ARGV5} CACHE STRING "Template params as key=value pairs")

if(template_file STREQUAL "" OR output_file STREQUAL "" OR replacements STREQUAL "")
    message(FATAL_ERROR "You must provide template_file, output_file and params")
endif()

file(READ ${template_file} file_content)

foreach(param IN LISTS params)
    string(REGEX MATCHALL "([^=]+)=([^=]+)" _ ${param})
    set(key ${CMAKE_MATCH_1})
    set(value ${CMAKE_MATCH_2})
    string(REGEX REPLACE "\\$\\{${key}\\}" "${value}" file_content "${file_content}")
endforeach()

file(WRITE ${output_file} "${file_content}")

message(STATUS "Template processing completed for: ${output_file}")