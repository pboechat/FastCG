#version 430 core

#ifdef VULKAN
#extension GL_GOOGLE_include_directive : enable
#endif

#include "FastCG.glsl"
#include "Material.glsl"

layout(location = 0) in vec3 vNormal;

layout(location = 0) out vec4 oDiffuse;
layout(location = 1) out vec4 oNormal;
layout(location = 2) out vec4 oSpecular;

void main()
{
	oDiffuse = uDiffuseColor;
	oNormal = vec4(PackNormalToColor(vNormal), 0.0);
	oSpecular = vec4(uSpecularColor.xyz, PackToNormalizedValue(uShininess));
}