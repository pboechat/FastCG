#version 430 core

#ifdef ENABLE_INCLUDE_EXTENSION_DIRECTIVE
#extension GL_GOOGLE_include_directive : enable 
#endif

#include "Lighting.glsl"
#include "FastCG.glsl"
#include "Scene.glsl"
#include "Instance.glsl"

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
	vec4 worldPosition = GetInstanceData().model * iPosition;
	vec3 viewPosition = vec3(uView * worldPosition);
	vLightDirection = normalize(uLight0ViewPosition.xyz - (step(0, GetLightType()) * viewPosition));
	vViewerDirection = normalize(-viewPosition);
	vPosition = worldPosition.xyz;
	vNormal = normalize(mat3(GetInstanceData().modelViewInverseTranspose) * iNormal);
	vUV = iUV;
	gl_Position = GetInstanceData().modelViewProjection * iPosition;
}