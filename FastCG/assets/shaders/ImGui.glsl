#ifndef FASTCG_IMGUI_CONSTANTS_GLSL
#define FASTCG_IMGUI_CONSTANTS_GLSL

#include "FastCG.glsl"

layout(BINDING(0, 0)) uniform ImGuiConstants
{
    mat4 uProjection;
};

#endif