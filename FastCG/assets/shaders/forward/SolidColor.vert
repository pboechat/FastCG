#version 430 core

#ifdef VULKAN
#extension GL_GOOGLE_include_directive : enable
#endif

#include "FastCG.glsl"
#include "InstanceConstants.glsl"

layout(location = 0) in vec4 iPosition;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec2 iUV;

layout(location = 0) out vec3 vViewerDirection;
layout(location = 1) out vec3 vVertexPosition;
layout(location = 2) out vec3 vNormal;
layout(location = 3) out vec2 vUV;

void main()
{
	vVertexPosition = vec3(uModelView * iPosition);
	vViewerDirection = normalize(-vVertexPosition);
	vNormal = normalize(mat3(uModelViewInverseTranspose) * iNormal);
	vUV = iUV;
	gl_Position = uModelViewProjection * iPosition;
}