#version 430 core

#ifdef VULKAN
#extension GL_GOOGLE_include_directive : enable
#endif

#include "FastCG.glsl"
#include "Scene.glsl"
#include "Instance.glsl"
#include "Lighting.glsl"

layout(location = 0) in vec4 iPosition;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec2 iUV;
layout(location = 3) in vec4 iTangent;

layout(location = 0) out vec3 vLightDirection;
layout(location = 1) out vec3 vViewerDirection;
layout(location = 2) out vec3 vPosition;
layout(location = 3) out vec2 vUV;

void main()
{
	vec3 normal = normalize(mat3(GetInstanceData().modelViewInverseTranspose) * iNormal);
	vec3 tangent = normalize(mat3(GetInstanceData().modelViewInverseTranspose) * iTangent.xyz);
	vec3 binormal = normalize(cross(normal, tangent) * iTangent.w);

	mat3 tangentSpaceMatrix = transpose(mat3(tangent, binormal, normal));

	vec4 worldPosition = GetInstanceData().model * iPosition;
	vec3 viewPosition = vec3(uView * worldPosition);
	vLightDirection = tangentSpaceMatrix * normalize(uLight0ViewPosition.xyz - (step(0, GetLightType()) * viewPosition));
	vViewerDirection = tangentSpaceMatrix * normalize(-viewPosition);
	vPosition = worldPosition.xyz;
	vUV = iUV;

	gl_Position = GetInstanceData().modelViewProjection * iPosition;
}