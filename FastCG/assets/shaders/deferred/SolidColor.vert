#version 430 core

#ifdef VULKAN
#extension GL_GOOGLE_include_directive : enable
#endif

#include "FastCG.glsl"
#include "Instance.glsl"

layout(location = 0) in vec4 iPosition;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec2 iUV;

layout(location = 0) out vec3 vNormal;

void main()
{
	vNormal = normalize(mat3(GetInstanceData().modelViewInverseTranspose) * iNormal);
	gl_Position = GetInstanceData().modelViewProjection * iPosition;
}