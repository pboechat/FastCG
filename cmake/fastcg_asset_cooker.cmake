cmake_minimum_required(VERSION 3.10)

# Usage: 
# cmake -P fastcg_asset_cooker.cmake "working_directory" "path/to/recipe" "path/to/output"

function(_cook_ktx)
    get_property(KTX_WRKDIR VARIABLE PROPERTY "working_directory")
    get_property(KTX_OUTPUT VARIABLE PROPERTY "output_file")

    find_program(KTX ktx "ktx command line tool")
    if(NOT KTX)
        message(FATAL_ERROR "ktx command not found")
    endif()

    get_property(KTX_SOURCE VARIABLE PROPERTY "source")
    if(NOT KTX_SOURCE)
        message(FATAL_ERROR "source required")
    endif()
    string(REGEX REPLACE "," ";" KTX_SOURCES "${KTX_SOURCE}")

    get_property(KTX_FORMAT VARIABLE PROPERTY "format")
    if(NOT KTX_FORMAT)
        message(FATAL_ERROR "format required")
    endif()

    list(APPEND KTX_ARGS --format ${KTX_FORMAT})

    get_property(KTX_MIPMAPS VARIABLE PROPERTY "mipmaps")
    if(KTX_MIPMAPS LESS 0)
        list(APPEND KTX_ARGS --generate-mipmap)
    elseif(KTX_MIPMAPS GREATER 0)
        list(APPEND KTX_ARGS --generate-mipmap --levels ${KTX_MIPMAPS})
    endif()

    get_property(KTX_LAYERS VARIABLE PROPERTY "layers")
    if(KTX_LAYERS)
        list(APPEND KTX_ARGS --layers ${KTX_LAYERS})
    endif()

    get_property(KTX_CUBEMAP VARIABLE PROPERTY "cubemap")
    if(KTX_CUBEMAP)
        list(APPEND KTX_ARGS --cubemap)
    endif()

    execute_process(
        COMMAND ${KTX} create ${KTX_ARGS} ${KTX_SOURCES} "${KTX_OUTPUT}"
        WORKING_DIRECTORY ${KTX_WRKDIR}
        RESULT_VARIABLE result
        ERROR_VARIABLE error
    )

    if(NOT result EQUAL 0)
        message(FATAL_ERROR "ktx finished with exit code ${result}.\nerror: ${error}")
    endif()
endfunction()

function(_cook_dds)
    get_property(DDS_WRKDIR VARIABLE PROPERTY "working_directory")
    get_property(DDS_OUTPUT VARIABLE PROPERTY "output_file")

    find_program(TEXCONV texconv "texconv command line tool")
    if(NOT TEXCONV)
        message(FATAL_ERROR "texconv command not found")
    endif()

    find_program(TEXASSEMBLE texassemble "texassemble command line tool")
    if(NOT TEXASSEMBLE)
        message(FATAL_ERROR "texassemble command not found")
    endif()

    get_property(DDS_SOURCE VARIABLE PROPERTY "source")
    if(NOT DDS_SOURCE)
        message(FATAL_ERROR "source required")
    endif()

    get_property(DDS_FORMAT VARIABLE PROPERTY "format")
    if(NOT DDS_FORMAT)
        message(FATAL_ERROR "format required")
    endif()

    list(APPEND TEXCONV_ARGS -f ${DDS_FORMAT})

    get_property(TEXCONV_MIPMAPS VARIABLE PROPERTY "mipmaps")
    if(TEXCONV_MIPMAPS)
        list(APPEND TEXCONV_ARGS -m)
    endif()

    get_property(DDS_ARRAY VARIABLE PROPERTY "array")
    get_property(DDS_CUBEMAP VARIABLE PROPERTY "cubemap")

    if(DDS_ARRAY)
        string(REGEX REPLACE "," ";" DDS_SOURCES "${DDS_SOURCE}")
        execute_process(
            COMMAND ${TEXASSEMBLE} array -f ${DDS_FORMAT} ${TEXASSEMBLE_ARGS} ${DDS_SOURCES} -o "${DDS_OUTPUT}" -y
            WORKING_DIRECTORY ${DDS_WRKDIR}
            ERROR_VARIABLE error
        )
    elseif(DDS_CUBEMAP)
        string(REGEX REPLACE "," ";" DDS_SOURCES "${DDS_SOURCE}")
        execute_process(
            COMMAND ${TEXASSEMBLE} cubemap -f ${DDS_FORMAT} ${TEXASSEMBLE_ARGS} ${DDS_SOURCES} -o "${DDS_OUTPUT}" -y
            WORKING_DIRECTORY ${DDS_WRKDIR}
            ERROR_VARIABLE error
        )
    else()
        execute_process(
            COMMAND ${TEXCONV} -f ${DDS_FORMAT} ${TEXCONV_ARGS} "${DDS_SOURCE}" -ft "${DDS_OUTPUT}" -y
            WORKING_DIRECTORY ${DDS_WRKDIR}
            ERROR_VARIABLE error
        )
    endif()

    if(NOT result EQUAL 0)
        message(FATAL_ERROR "texconv/texassemble finished with exit code ${result}.\nerror: ${error}")
    endif()
endfunction()

set(working_directory ${CMAKE_ARGV3})
set(recipe_file ${CMAKE_ARGV4})
set(output_file ${CMAKE_ARGV5})

if(working_directory STREQUAL "" OR recipe_file STREQUAL "" OR output_file STREQUAL "")
    message(FATAL_ERROR "You must provide working_directory, recipe_file and output_file")
endif()

file(READ ${recipe_file} recipe_content)
string(REGEX REPLACE "\n" ";" lines "${recipe_content}")

foreach(line IN LISTS lines)
    string(REGEX MATCHALL "([^=]+)=(.+)" _ "${line}")
    set(key ${CMAKE_MATCH_1})
    set(value ${CMAKE_MATCH_2})
    if(NOT key OR NOT value)
        continue()
    endif()
    set(${key} ${value})
endforeach()

if(recipe_file MATCHES "\\.ktx.recipe$")
    _cook_ktx()
elseif(recipe_file MATCHES "\\.dds.recipe$")
    _cook_dds()
else()
    message(FATAL_ERROR "Don't know how to cook ${recipe_file}")
endif()

message("Cooked ${recipe_file}")