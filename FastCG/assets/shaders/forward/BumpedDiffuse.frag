#version 430 core

#ifdef VULKAN
#extension GL_GOOGLE_include_directive : enable
#endif

#include "FastCG.glsl"
#include "LightingConstants.glsl"
#include "MaterialConstants.glsl"
#include "../Lighting.glsl"

layout(location = 0) in vec3 vLightDirection;
layout(location = 1) in vec3 vPosition;
layout(location = 2) in vec2 vUV;

layout(location = 0) out vec4 oColor;

void main()
{
	vec4 diffuse = uDiffuseColor * texture(uColorMap, (vUV * uColorMapTiling));
	vec3 normal = UnpackNormalFromColor(texture(uBumpMap, (vUV * uBumpMapTiling))).xyz;
	oColor = Lighting(diffuse, vLightDirection, vPosition, normal);
}