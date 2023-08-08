#version 430 core

#ifdef ENABLE_INCLUDE_EXTENSION_DIRECTIVE
#extension GL_GOOGLE_include_directive : enable 
#endif

#include "FastCG.glsl"

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec2 iUV;

layout(location = 0) out vec2 vUV;

void main()
{
	vUV = iUV;
#if VULKAN
	vUV.y = 1 - vUV.y;
#endif
	gl_Position = vec4(iPosition, 1);
}