#version 430 core

#ifdef ENABLE_INCLUDE_EXTENSION_DIRECTIVE
#extension GL_GOOGLE_include_directive : enable 
#endif

#include "../FastCG.glsl"
#include "ShadowMapPass.glsl"

layout(location = 0) in vec4 iPosition;

void main()
{
    gl_Position = GetShadowMapPassInstanceData().modelViewProjection * iPosition;
}