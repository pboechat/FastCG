#version 430 core

#ifdef ENABLE_INCLUDE_EXTENSION_DIRECTIVE
#extension GL_GOOGLE_include_directive : enable 
#endif

#include "FastCG.glsl"
#include "Instance.glsl"

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec4 iTangent;
layout(location = 3) in vec2 iUV;

layout(location = 0) out vec3 vNormal;
layout(location = 1) out vec4 vTangent;
layout(location = 2) out vec2 vUV;

void main()
{
	vNormal = normalize(mat3(GetInstanceData().modelViewInverseTranspose) * iNormal);
	vTangent = vec4(normalize(mat3(GetInstanceData().modelViewInverseTranspose) * iTangent.xyz), iTangent.w);
	vUV = iUV;
	gl_Position = GetInstanceData().modelViewProjection * vec4(iPosition, 1);
}