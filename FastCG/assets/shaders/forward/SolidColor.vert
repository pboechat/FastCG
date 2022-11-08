#version 430 core

#ifdef VULKAN
#extension GL_GOOGLE_include_directive : enable
#endif

#include "FastCG.glsl"
#include "SceneConstants.glsl"
#include "LightingConstants.glsl"
#include "InstanceConstants.glsl"

layout(location = 0) in vec4 iPosition;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec2 iUV;

layout(location = 0) out vec3 vLightDirection;
layout(location = 1) out vec3 vViewerDirection;
layout(location = 2) out vec3 vPosition;
layout(location = 3) out vec3 vNormal;
layout(location = 4) out vec2 vUV;

void main()
{
	vec4 modelPosition = uModel * iPosition;
	vec3 viewPosition = vec3(uView * modelPosition);
	vLightDirection = normalize(uLight0ViewPosition.xyz - (step(0, FASTCG_LIGHT_TYPE()) * viewPosition));
	vViewerDirection = normalize(-viewPosition);
	vPosition = modelPosition.xyz;
	vNormal = normalize(mat3(uModelViewInverseTranspose) * iNormal);
	vUV = iUV;
	gl_Position = uModelViewProjection * iPosition;
}