cmake_minimum_required(VERSION 3.8)

if(FASTCG_RENDERING_SYSTEM STREQUAL "OpenGL")
    add_subdirectory(glew)
endif()
add_subdirectory(glm)
add_subdirectory(imgui)
add_subdirectory(stb)
add_subdirectory(tinyobj_loader_c)