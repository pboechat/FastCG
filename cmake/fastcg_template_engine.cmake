cmake_minimum_required(VERSION 3.10)

# Usage: 
# cmake -P fastcg_template_engine.cmake "path/to/template" "path/to/output" "key1=value1;key2=value2;key3=value3"

set(template_file ${CMAKE_ARGV3})
set(output_file ${CMAKE_ARGV4})

foreach(idx RANGE 5 ${CMAKE_ARGC})
    list(APPEND params "${CMAKE_ARGV${idx}}")
endforeach()

if(template_file STREQUAL "" OR output_file STREQUAL "" OR params)
    message(FATAL_ERROR "You must provide template_file, output_file and params")
endif()

file(READ ${template_file} file_content)

foreach(param IN LISTS params)
    string(REGEX MATCHALL "([^=]+)=([^=]+)" _ "${param}")
    set(key ${CMAKE_MATCH_1})
    set(value ${CMAKE_MATCH_2})
    set(pattern "\\$\\{${key}\\}")
    string(REGEX REPLACE ${pattern} "${value}" file_content "${file_content}")
endforeach()

file(WRITE ${output_file} "${file_content}")

message(STATUS "Template processing completed for: ${output_file}")