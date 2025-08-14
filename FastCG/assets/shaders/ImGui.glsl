#ifndef FASTCG_IMGUI_CONSTANTS_GLSL
#define FASTCG_IMGUI_CONSTANTS_GLSL

#include "FastCG.glsl"

layout(std140, BINDING_0_0) uniform ImGuiConstants
{
    mat4 uProjection;
};

#endif