#version 430 core

#ifdef VULKAN
#extension GL_GOOGLE_include_directive : enable
#endif

#include "FastCG.glsl"
#include "Scene.glsl"
#include "Lighting.glsl"
#include "Instance.glsl"

layout(location = 0) in vec4 iPosition;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec2 iUV;

layout(location = 0) out vec3 vLightDirection;
layout(location = 1) out vec3 vPosition;
layout(location = 2) out vec3 vNormal;
layout(location = 3) out vec2 vUV;

void main()
{
	vec4 worldPosition = uModel * iPosition;
	vec3 viewPosition = vec3(uView * worldPosition);
	vLightDirection = normalize(uLight0ViewPosition.xyz - (step(0, GetLightType()) * viewPosition));
	vPosition = worldPosition.xyz;
	vNormal = normalize(mat3(uModelViewInverseTranspose) * iNormal);
	vUV = iUV;
	gl_Position = uModelViewProjection * iPosition;
}