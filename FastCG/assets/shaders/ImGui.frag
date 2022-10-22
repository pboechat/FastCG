#version 430 core

#ifdef VULKAN
#extension GL_GOOGLE_include_directive : enable
#endif

#include "FastCG.glsl"

layout(BINDING(0, 0)) uniform sampler2D uColorMap;

layout(location = 0) in vec2 vUV;
layout(location = 1) in vec4 vColor;

layout(location = 0) out vec4 oColor;

void main()
{
    oColor = texture(uColorMap, vUV) * vColor;
}